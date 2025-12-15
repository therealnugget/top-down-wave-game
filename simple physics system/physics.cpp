#include "physics.hpp"
#include "main.hpp"
#include "multicast delegates.hpp"
#include "debug.hpp"
#include "math.hpp"
#include "Item.hpp"
#include <limits>
#include <intrin.h>
#include <unordered_map>
#include <variant>
rbList *Physics::entityHead = nullptr;
Node<Entity *> *Physics::standaloneEntityHead = nullptr;
Node<Text *> *Physics::textHead = nullptr;
static Node<Entity*>* curStandalone;
static Node<Text*>* curText;
rbListList *Physics::sortedEntityHeads = nullptr;
rbListList *Physics::unsortedEntityHeads = nullptr;
EmptyStack<RigidBody*> Physics::sortedCacheNodes = EmptyStack<RigidBody *>();
uint Physics::numCacheNodes = 0;
int Physics::numEntities = 0;
int Physics::sectionSize = 0;
int Physics::numSections = 0;
int Physics::excessEntityNo = 0;
int Physics::numUnsortedEntities = 0;
int Physics::numUnsortedSections = 0;
int Physics::unsortedSectionSize = 0;
int Physics::unsortedExcessEntityNo = 0;
ushort Physics::frameInd = 0;
const std::initializer_list<FVector2> Physics::DefaultSquareVerticesAsList = {
	{ -.5f, -.5f },{ -.5f, .5f },{ .5f, .5f },{ .5f, -.5f },
};
const std::initializer_list<FVector2> SubRBData::DefaultSquareVerticesAsList = {
	{ -.5f, -.5f },{ -.5f, .5f },{ .5f, .5f },{ .5f, -.5f },
};
const std::vector<FVector2> Physics::DefaultSquareVerticesVec = {
	{ -.5f, -.5f },{ -.5f, .5f },{ .5f, .5f },{ .5f, -.5f },
};
//imageDimensions is to resize the collider based on how big the contents of the image is in comparison to the image itself
Node<RigidBody*>* Physics::SubscribeEntity(const std::string& basePath, const std::vector<const char*>& animPaths, std::vector<FVector2> _narrowPhaseVertices, FVector2 startPos, IntVec2 size, std::initializer_list<FVector2> _centreOfRot, FVector2 _centreOfRotNPVert, IntVec2 _renderOffset, int tag, std::function<void(Collision*)> collisionCallback, std::unordered_map<std::string, std::variant<FVector2, FVector2*>> imageSizes, std::unordered_map<std::string, bool> isGlobalSize, FVector2 initVel, float angle, float mass, bool moveable, bool isTrigger, const std::initializer_list<const char*>& endPaths) {
	auto data = SubRBData(basePath, animPaths, _narrowPhaseVertices, startPos, size, _centreOfRot, _centreOfRotNPVert, _renderOffset, tag, collisionCallback, imageSizes, isGlobalSize, initVel, angle, mass, moveable, isTrigger, endPaths);
	return SubscribeEntity(&data);
}
Node<RigidBody*>* Physics::SubscribeEntity(SubRBData *data) {
	return SubscribeEntity(new RigidBody(data));
}
Node<RigidBody*> *Physics::SubscribeEntity (RigidBody *rb){
	static uint totalNumEntities = 0;
	rb->entityIndex = totalNumEntities++;
	return Node<RigidBody*>::AddAtHead(rb, &entityHead);
}
Node<Entity*>* Physics::SubStandaloneEnt(Entity* entity) {
	return Node<Entity*>::AddAtHead(entity, &standaloneEntityHead);
}
Node<Text*>* Physics::SubText(Text* text) {
	return Node<Text*>::AddAtHead(text, &textHead);
}
//doesn't delete value of node
Node<Text*>* Physics::UnSubText(Node<Text*> *text) {
	return Node<Text*>::Remove(&textHead, text);
}
Node<Entity*>* Physics::SubStandaloneEnt(SubRBData *data) {
	return SubStandaloneEnt(new Entity(data));
}
//deletes value of node
Node<Entity*>* Physics::UnsubStandaloneEnt(Node<Entity*>* remove) {
	return Node<Entity*>::RemoveWVal(&standaloneEntityHead, remove);
}
void Physics::UnSubscribeEntity(rbList* node) {
	rbList::Remove(&entityHead, node);
	numEntities--;
}
//don't delete rigidbodies directly; use this func instead
void Physics::DeleteRB(rbList* node) {
	delete node->value;
	UnSubscribeEntity(node);
}
Node<RigidBody*>* Physics::StandaloneRB(IntVec2 size, FVector2 startPos, int tag, CollisionCallback collisionCallback, int_fast64_t layer, bool isTrigger, float mass, bool moveable, FVector2 _centreOfRotNPVert, FVector2 initVel, float angle) {
	auto data = SubRBData(Main::empty_string, std::vector<const char*>(), size * Physics::DefaultSquareVerticesVec, startPos, IntVec2::One, std::initializer_list<FVector2>(), _centreOfRotNPVert, IntVec2::Zero, tag, collisionCallback, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), initVel, angle, mass, moveable, isTrigger, std::initializer_list<const char*>(), false, .0f, layer);
	RigidBody* rb = new RigidBody(&data);
	return SubscribeEntity(rb);
}
template <typename T>
Vector2<T> Vector2<T>::FromTo(RigidBody* from, RigidBody* to) {
	return FromTo(from->GetPosition(), to->GetPosition());
}
static rbList* curNode;
const FVector2 FVector2::One = FVector2(1.f, 1.f);
const FVector2 FVector2::Zero = {};
const FVector2 FVector2::Right = FVector2(1.f, .0f);
const FVector2 FVector2::Left = FVector2(-1.f, .0f);
const FVector2 FVector2::Up = FVector2(.0f, 1.f);
const FVector2 FVector2::Down = FVector2(.0f, -1.f);
const IntVec2 IntVec2::One = static_cast<IntVec2>(FVector2::One);
const IntVec2 IntVec2::Zero = static_cast<IntVec2>(FVector2::Zero);
const IntVec2 IntVec2::Right = static_cast<IntVec2>(FVector2::Right);
const IntVec2 IntVec2::Left = static_cast<IntVec2>(FVector2::Left);
const IntVec2 IntVec2::Up = static_cast<IntVec2>(FVector2::Up);
const IntVec2 IntVec2::Down = static_cast<IntVec2>(FVector2::Down);
const float Physics::fricCoef = 20000.f;
const FVector2 Physics::frictionVec = (FVector2)FVector2::One * fricCoef;
const float Physics::frictionMagnitude = GetFrictionVec().Magnitude();
static FVector2 curVelSign;
//should be called after all behaviours' updates are called so transformations from this frame's position and rotation changes, and force calculations can be enacted
static FVector2 curFriction;
static float fricCoefByDT;
void RigidBody::SetInitCOR() {
	coefRestitution = Physics::defaultCOR;
}
/*
template<>
struct std::hash<Physics::DoubleRB> {
	std::size_t operator()(const Physics::DoubleRB &doubleRB) const {
		return (std::hash<void*>()(doubleRB.a) >> 1) ^ (std::hash<void*>()(doubleRB.b));
	}
};
std::unordered_map<Physics::DoubleRB, std::mutex> collidingRBs = std::unordered_map<Physics::DoubleRB, std::mutex>();
*/
std::mutex outerMutex;
void Physics::NarrowPhase(RigidBody* a, RigidBody *b
#ifdef IS_MULTI_THREADED
	, int index
#endif
) {
	FVector2 normal = FVector2::Zero;
	float depth =
#ifdef DEBUG_BUILD
		FLT_MAX
#else
		std::numeric_limits<float>::max()
#endif
		;
	{
		std::lock_guard<std::mutex> lockOuterMutex(outerMutex);
		a->checkColliding.lock();
		b->checkColliding.lock();
	}
	//this is a failed solution i tried of putting collisions into a map to check if two colliders had collided before trying to recalculate the collision.
	//as it turns out, it is actually faster to recalculate the extra ~19 collisions every frame than it is to check the unordered map every time.
	/*
	uint_fast64_t collisionIndexAB = RigidBody::GetDoubleEntColIndex(a, b), collisionIndexBA = RigidBody::GetDoubleEntColIndex(b, a);
	if (a->GetEntityCollided(collisionIndexAB, collisionIndexBA) || b->GetEntityCollided(collisionIndexAB, collisionIndexBA)) goto free_mutex;
	a->SetEntitiesCollided(collisionIndexAB);*/
	{
		int i;
		float minA, maxA;
		float minB, maxB;
		bool colliding = true;
		FVector2 vec;
		FVector2 vecB, edge, axis;
		float axisDepth;
		auto checkProj = [&](RigidBody* rb) -> void {
			FVector2* vertices = rb->verticesNarrowP;
			uint numVertices = rb->numNarrowPhaseVertices;
			for (i = 0; i < numVertices; i++) {
				vecB = vertices[(i + 1) % numVertices];
				edge = vecB - vertices[i];
				auto r = sqrtf(edge.x * edge.x + edge.y * edge.y);
				if (r == .0f) r = 1.0f;
				axis.x = -edge.y / r;
				axis.y = edge.x / r;
				ProjectVertices(a->verticesNarrowP, a->numNarrowPhaseVertices, axis, minA, maxA);
				ProjectVertices(b->verticesNarrowP, b->numNarrowPhaseVertices, axis, minB, maxB);
				colliding &= minA < maxB && minB < maxA;
				if (!colliding) return;
				axisDepth = fminf(maxB - minA, maxA - minB);
				if (axisDepth < depth) {
					depth = axisDepth;
					normal = axis;
				}
			}
			};
		checkProj(a);
		float dot = .0f;
		bool triggerCollision = a->bIsTrigger || b->bIsTrigger;
		if (!colliding) goto free_mutex;

		{
			checkProj(b);
			FVector2 aPos = a->position, bPos = b->position;
			if ((FVector2::FromTo(aPos, bPos) ^ normal) < .0f) normal *= -1.f;
			bool aMoveable = a->bMoveable, bMoveable = b->bMoveable;
			constexpr int last_bit_offset = 63;
			if (triggerCollision) goto ret;
			if (!(a->layer & b->layer) || a->layer == b->layer && (!(a->layer << last_bit_offset) || !(b->layer << last_bit_offset))) goto free_mutex;

			{
				FVector2 offset = normal * depth * .5f;
				a->position = aPos - offset * aMoveable - offset * !bMoveable;
				b->position = bPos + offset * bMoveable + offset * !aMoveable;
				if (aMoveable) AdjustColVertices(a);
				if (bMoveable) AdjustColVertices(b);
				FVector2 relVel = b->GetVelocity() - a->GetVelocity();
				dot = normal ^ relVel;
			}
			if (dot > .0f) {
				goto ret;
			}
			{
				float e = std::fminf(a->GetCOR(), b->GetCOR());
				FVector2 jn = normal * dot * -(1.f + e) / ((a->GetInvMass() + b->GetInvMass()));
				if (aMoveable) a->velocity -= jn * a->GetInvMass();
				if (bMoveable) b->velocity += jn * b->GetInvMass();
			}
		}
	ret:;
		if (colliding/* && !a->GetEntityCollided(collisionIndexAB, collisionIndexBA, true) && !b->GetEntityCollided(collisionIndexAB, collisionIndexBA, true)*/) {
			//a->SetEntitiesCollided(collisionIndexBA, true);
			Collision *collisionData = nullptr;
			if (a->OnCollision) {
				collisionData = static_cast<Collision*>(_malloca(sizeof(Collision)));
				*collisionData = Collision(b, dot, triggerCollision, normal);
				a->OnCollision(collisionData);
			}
			if (!b->OnCollision) {
				_freea(collisionData);
				goto free_mutex;
			}
			if (!collisionData) {
				collisionData = static_cast<Collision*>(_malloca(sizeof(Collision)));
				*collisionData = Collision(a, dot, triggerCollision, normal);
			}
			else collisionData->SetCollider(a);
			b->OnCollision(collisionData);
			_freea(collisionData);
		}
	}
	free_mutex:
	a->checkColliding.unlock();
	b->checkColliding.unlock();
}
void Physics::ProjectVertices(FVector2 *vertices, uint numVertices, FVector2 axis, float &min, float &max) {
	ASMSetProjection(vertices, &axis, numVertices, &min, &max);
	//translated back to c++ (almost the exact same code. for language-specific reasons, it is not quite exactly the same, there's a small difference.)
	/*register float projection;
	min = std::numeric_limits<float>::max();
	max = std::numeric_limits<float>::min();
	for (register int i = 0; i < numVertices; i++) {
		projection = Projection(vertices[i], axis);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}*/
}
void Physics::OuterBroadPhase(bool searchSortedList) {
#ifndef IS_MULTI_THREADED
	rbListList *curEntHeadList = searchSortedList ? sortedEntityHeads : unsortedEntityHeads;
	while (curEntHeadList) {
		BroadPhase(curEntHeadList->value);
		rbListList::Advance(&curEntHeadList);
	}
#endif
}
void Physics::BroadPhase(Node<RigidBody *> *rbNode
#ifdef IS_MULTI_THREADED
	, int index
#endif
) {
	Node<RigidBody *> *curMTNode = rbNode;
	Node<RigidBody*> *curInnerNode;
	while (curMTNode) {
		curInnerNode = curMTNode->GetNext();
		while (curInnerNode) {
			if (curInnerNode->value != curMTNode->value) NarrowPhase(curInnerNode->value, curMTNode->value
#ifdef IS_MULTI_THREADED
				, index
#endif
			);
			Node<RigidBody*>::Advance(&curInnerNode);
		}
		Node<RigidBody*>::Advance(&curMTNode);
	}
}
void Physics::ThreadFunc(int index) {
#ifdef DEBUG_BUILD
	if (index >= thread_count) {
		ThrowError("index cannot be greater than or equal to thread count.");
		return;
	}
#endif
	for (;;) {
		{
			std::unique_lock<std::mutex> lock(threadFuncMutexes[index]);
			while (!canRunThread[index] && !stopThread[index]) threadFuncConds[index].wait(lock);
			if (stopThread[index]) return;
		}
		auto DoBP = [index](rbListList* curMtList, int secSize, int excessEntNo, int numSecs) -> void {
			int i;
			for (i = 0; i < secSize * index; i++) {
				//"go to after."
				//"after what?"
				//"after after."
				if (!curMtList) return;
				rbListList::Advance(&curMtList);
			}
			for (i = 0; i < secSize + excessEntNo * (index == (numSecs - 1)); i++) {
				if (!curMtList) return;
				BroadPhase(curMtList->value
#ifdef IS_MULTI_THREADED
					, index
#endif
				);
				rbListList::Advance(&curMtList);
			}
			};
		DoBP(sortedEntityHeads, sectionSize, excessEntityNo, numSections);
		DoBP(unsortedEntityHeads, unsortedSectionSize, unsortedExcessEntityNo, numUnsortedSections);
		//printf("index is %d, number of sections is %d, section size is %d\n", index, numSections, sectionSize);
		{
			std::unique_lock<std::mutex> lock(mainWaitMutexes[index]);
			canRunThread[index] = false;
		}
		mainWaitConds[index].notify_one();
	}
}
//needs to be called before the first physics simulation tick. if the program is multithreaded, because it initializes the thread pool
void Physics::Init() {
	Main::dtUpdates += Physics::Update;
#ifdef IS_MULTI_THREADED
	for (int i = 0; i < thread_count; i++) {
		workers[i] = std::thread(ThreadFunc, i);
	}
#endif
}
std::atomic<bool> Physics::canRunThread[thread_count] = { 0 };
bool Physics::stopThread[thread_count] = { 0 };
std::condition_variable Physics::threadFuncConds[thread_count];
std::condition_variable Physics::mainWaitConds[thread_count];
std::mutex Physics::threadFuncMutexes[thread_count];
std::mutex Physics::mainWaitMutexes[thread_count];
std::thread Physics::workers[thread_count];
int Physics::threadIndex;
static FVector2 currentRBPos, currentRBVel;
static RigidBody *currentRB;
static Entity *currentEntity;
static FVector2 maxNarrowPhase, minNarrowPhase;
static FVector2 *curNarrowPVert;
static uint curVertIndex;
int Physics::moveItrIndex;
static SDL_Rect* curRect;
static bool lastMovementItr;
QuadNode<RigidBody*> Physics::quadRoot = QuadNode<RigidBody *>(Physics::initCellAABB);
void Physics::DeleteQuadEntities(QuadNode<RigidBody*>* tree, bool isRoot) {
	rbList* treeValHead = tree->values;
	rbList* next;
	while (treeValHead) {
		next = treeValHead->GetNext();
		rbList::Remove(&tree->values, treeValHead, false);
		numCacheNodes = 0;
		treeValHead->value->cacheNodeRef = sortedCacheNodes.PushNode(treeValHead);
		numCacheNodes++;
		treeValHead = next;
	}
	if (tree->IsLeafNode()) {
		goto ret;
	}
	{
		DeleteQuadEntities(tree->GetBottomLeft());
		DeleteQuadEntities(tree->GetBottomRight());
		DeleteQuadEntities(tree->GetTopLeft());
		DeleteQuadEntities(tree->GetTopRight());
	}
ret:
	if (isRoot) {
		tree->ClearNodes();
		return;
	}
	delete tree;
}
void Physics::SortEntity(QuadNode<RigidBody*>* quadNode, Node<RigidBody *> *entities, int currentDepth) {
	int noEntInCurCell = 0;
	RigidBody* curRB;
	rbList* unsortedRbs = nullptr;
	while (entities) {
		curRB = entities->value;
		if (Physics::EntityInBoxBroadPhase(quadNode->aabb, curRB->broadPhaseAABB)) {
			if (sortedCacheNodes.IsEmpty()) rbList::AddAtHead(curRB, &quadNode->values);
			else rbList::AddAtHead(curRB, &quadNode->values, sortedCacheNodes.PopNode());
			noEntInCurCell++;
		}
		else if (currentDepth == 0) {
			curRB->cacheNodeRef = nullptr;
			rbList::AddAtHead(curRB, &unsortedRbs);
		}
		Node<RigidBody*>::Advance(&entities);
	}
	auto AddRbsToNPList = [unsortedRbs, noEntInCurCell](rbList* vals, rbListList** head, int &numEnts, int &secSize, int &excessEntNo, int &numSecs) -> void {
		rbListList::AddAtHead(vals, head);
		numEnts += noEntInCurCell;
		secSize = numEnts / thread_count;
		excessEntNo = numEnts % thread_count;
		numSecs = Math::Min<int>(thread_count, numEnts);
		};
	if (unsortedRbs) AddRbsToNPList(unsortedRbs, &unsortedEntityHeads, numUnsortedEntities, unsortedSectionSize, unsortedExcessEntityNo, numUnsortedSections);
	auto& vals = quadNode->values;
	if (!vals || noEntInCurCell == 1) return;
	if (noEntInCurCell < Physics::max_ent_per_cell || currentDepth >= Physics::max_quadtree_depth - 1) {
		AddRbsToNPList(vals, &sortedEntityHeads, numEntities, sectionSize, excessEntityNo, numSections);
		return;
	}
	quadNode->CreateChildNodes();
#ifdef SHOW_QUAD_TREE
	FVector2 mid = (quadNode->GetAABB().minimum + quadNode->GetAABB().maximum) * .5f, min = quadNode->GetAABB().minimum, max = quadNode->GetAABB().maximum;
	boundsArr.push_back({ { mid.x, min.y }, { mid.x, max.y } });
	boundsArr.push_back({ { min.x, mid.y }, { max.x, mid.y } });
#endif
	auto newDepth = currentDepth + 1;
	auto& values = quadNode->values;
	SortEntity(quadNode->bottomLeft, values, newDepth);
	SortEntity(quadNode->bottomRight, values, newDepth);
	SortEntity(quadNode->topRight, values, newDepth);
	SortEntity(quadNode->topLeft, values, newDepth);
}
//slow af. matrix rotation multiplication, then adds the position to the vertices and it's O(n) where n is num vertices for the rb. default is pos and rot, otherwise specified just rot.
void Physics::AdjustColVertices(RigidBody* rb) {
	FVector2* curVertex;
	FVector2 currentVertexWithoutPos;
	FVector2 currentCentreOfNPVertRot;
	FVector2 vertexBeforeRotCalc;
	auto currentRotation = static_cast<float>(rb->rotation) * Physics::deg2rad;
	auto sinB = sinf(currentRotation), cosB = cosf(currentRotation);
	for (uint i = 0; i < rb->numNarrowPhaseVertices; i++) {
		curVertex = &rb->verticesNarrowP[i];
		currentVertexWithoutPos = rb->origNarrowPVertices[i];
		if (rb->entity) {
			currentVertexWithoutPos.x *= rb->entity->rect->w;
			currentVertexWithoutPos.y *= rb->entity->rect->h;
		}
		currentCentreOfNPVertRot = rb->centreOfNarrowPVertRot;
		vertexBeforeRotCalc = currentVertexWithoutPos - currentCentreOfNPVertRot;
		/*
		(x, y) --> (ycos(B) - ysin(B), xSin(B) + ycos(B))
		*/
		curVertex->x = vertexBeforeRotCalc.x * cosB - vertexBeforeRotCalc.y * sinB + rb->position.x + currentCentreOfNPVertRot.x;
		curVertex->y = vertexBeforeRotCalc.x * sinB + vertexBeforeRotCalc.y * cosB + rb->position.y + currentCentreOfNPVertRot.y;
	}
}
void Main::AssignIfMore(Vector2<float>& check, Vector2<float>& assignConditionally) {
	AssignIfMore(check.x, assignConditionally.x);
	AssignIfMore(check.y, assignConditionally.y);
}
void Main::AssignIfLess(Vector2<float>& check, Vector2<float>& assignConditionally) {
	AssignIfLess(check.x, assignConditionally.x);
	AssignIfLess(check.y, assignConditionally.y);
}
static int animFramesPassed;
void Physics::ProcessTexs() {
	if (!currentEntity) return;
	curRect = currentEntity->rect;
	if (currentRB) {
		currentRB->position.IntoRectXY(curRect);
		curRect->x += currentEntity->renderOffset.x + currentEntity->renderOffsetChangeX;
		curRect->y += currentEntity->renderOffset.y;
	}
	if (currentEntity->currentAnimation != -1) {
		animFramesPassed = 0;
		auto animTime = Animator::default_anim_time / currentEntity->animSpeed;
		if (currentEntity->animTime <= Animator::neg_anim_time / currentEntity->animSpeed) {
			//-= because the int cast returns a negative value
			animFramesPassed -= static_cast<int>(currentEntity->animTime / animTime);
			currentEntity->animTime += animFramesPassed * animTime;
		}
		currentEntity->animTime -= Main::DeltaTime() * currentEntity->animSpeed;
		if (currentEntity->animTime <= .0f) {
			currentEntity->animTime += animTime;
			animFramesPassed++;
		}
		if (!currentEntity->GetLooping() && (currentEntity->GetAnimFrame() + animFramesPassed >= currentEntity->GetNumAnimFrames() || currentEntity->animFrameIndex == -1)) {
			currentEntity->animFrameIndex = -1;
		}
		else if (animFramesPassed && currentEntity->anims.size() > 0) {
			currentEntity->SetNextAnimTex(animFramesPassed);
			static std::variant<IntVec2, IntVec2*>* imageSizes;
			imageSizes = currentEntity->imageSizes;
			if (imageSizes) {
				static int currentAnim;
				currentAnim = currentEntity->currentAnimation;
				static int baseAnim;
				baseAnim = currentAnim / static_cast<int>(Main::num_directions);
				if (currentEntity->isGlobalSize[baseAnim]) {
					static_cast<IntVec2>(std::get<IntVec2>(imageSizes[baseAnim])).IntoRectWH(curRect);
				}
				else static_cast<IntVec2*>(std::get<IntVec2*>(imageSizes[baseAnim]))[currentAnim - baseAnim * Main::num_directions].IntoRectWH(curRect);
			}
		}
	}
	//can only render single-threaded. T-T
	SDL_RenderCopyEx(Main::renderer, currentEntity->texture, nullptr, curRect, currentEntity->angle, currentEntity->centreOfRotation, currentEntity->flip);
	if (!currentEntity || !currentEntity->bRecordAnim) return;
	currentEntity->pastAnimation = currentEntity->currentAnimation;
}
void Physics::Update(float dt) {
	curNode = entityHead;
	while (curNode) {
		currentRB = curNode->value;
		//F_net=m*a_net
		//a_net=F_net/m
		//v=u+a_net*t
		//v=u+F_net/m*t
		//at=v-u
		//a=(v-u)/t
		//s = int(v)dt
		//=vt + f (where c is the position last frame)
		//let t be the time since the last frame, a_net is the acceleration calculated from the mass and the net force applied since the first frame
		//then
		//v=velocity from last frame + net force applied from last frame / mass of entity * delta time
		//v=velocity from last frame + net force applied from last frame * inverse mass of entity * delta time
		//can't add the force after the total force is added to the velocity, otherwise the force will be reset before the start of the next frame.
		currentRBVel = currentRB->GetVelocity();
		if (currentRBVel.Magnitude() > (fricCoefByDT = fricCoef * dt)) currentRB->velocity -= Math::SignOrZero(currentRBVel) * fricCoefByDT * currentRB->friction;
		else currentRB->velocity = FVector2::Zero;
		currentRB->velocity += currentRB->force * currentRB->invMass * dt;
		currentRB->difPositionSection = (currentRB->newPosition - currentRB->pastPosition) * inv_num_movement_iterations_f;
		AdjustColVertices(currentRB);
		Node<RigidBody*>::Advance(&curNode);
	}
	for (moveItrIndex = 0; moveItrIndex < num_movement_iterations; moveItrIndex++) {
		curNode = entityHead;
		while (curNode) {
			currentRB = curNode->value;
			if (moveItrIndex == 0) {
				currentRB->difPositionSection += currentRB->velocity * dt * inv_num_movement_iterations_f;
			}
			for (curVertIndex = 0; curVertIndex < currentRB->numNarrowPhaseVertices; curVertIndex++) {
				curNarrowPVert = &currentRB->GetNarrowPhaseVertices()[curVertIndex];
				*curNarrowPVert += currentRB->difPositionSection;
				if (curVertIndex == 0) {
					minNarrowPhase = *curNarrowPVert;
					maxNarrowPhase = *curNarrowPVert;
					continue;
				}
				Main::AssignIfMore(*curNarrowPVert, maxNarrowPhase);
				Main::AssignIfLess(*curNarrowPVert, minNarrowPhase);
			}
			currentRB->broadPhaseAABB = { minNarrowPhase, maxNarrowPhase };
			currentRB->position += currentRB->difPositionSection;
			//currentRB->ClearCollidedEntities();
			Node<RigidBody*>::Advance(&curNode);
		}
		numEntities = 0;
		sectionSize = 0;
		numSections = 0;
		excessEntityNo = 0;
		numUnsortedEntities = 0;
		unsortedSectionSize = 0;
		numUnsortedSections = 0;
		unsortedExcessEntityNo = 0;
		SortEntity(&quadRoot, entityHead);
#ifdef IS_MULTI_THREADED
		for (threadIndex = 0; threadIndex < thread_count; threadIndex++) {
			{
				std::unique_lock<std::mutex> threadFuncLG(threadFuncMutexes[threadIndex]);
				canRunThread[threadIndex] = true;
			}
			threadFuncConds[threadIndex].notify_one();
		}
		for (threadIndex = 0; threadIndex < thread_count; threadIndex++) {
			std::unique_lock<std::mutex> lock(mainWaitMutexes[threadIndex]);
			while (canRunThread[threadIndex]) {
				mainWaitConds[threadIndex].wait(lock);
			}
		}
#else
		OuterBroadPhase();
		OuterBroadPhase(true);
#endif
		DeleteQuadEntities(&quadRoot, true);
		rbListList::RemoveAllNodes(&sortedEntityHeads, true);
		rbListList::RemoveAllNodes(&unsortedEntityHeads, [](rbList** list) {
			rbList::RemoveAllNodes(list, true);
			}, true);
	}
	if (frameInd++ == cache_flush_interval && numCacheNodes > numEntities) {
		sortedCacheNodes.Flush();
		frameInd = 0;
	}
	curNode = entityHead;
	while (curNode) {
		currentRB = curNode->value;
		currentEntity = currentRB->entity;
		ProcessTexs();
		//currentRB->ClearCollidedEntities(true);
		currentRB->force = FVector2::Zero;
		currentRB->pastPosition = currentRB->position;
		currentRB->newPosition = currentRB->pastPosition;
		if (!frameInd) {
			currentRB->cacheNodeRef = nullptr;
		}
		Node<RigidBody*>::Advance(&curNode);
	}
	curStandalone = standaloneEntityHead;
	while (curStandalone) {
		currentRB = nullptr;
		currentEntity = curStandalone->value;
		ProcessTexs();
		Node<Entity*>::Advance(&curStandalone);
	}
	curText = textHead;
	while (curText) {
		curText->value->RenderText();
		Node<Text*>::Advance(&curText);
	}
#ifdef SHOW_QUAD_TREE
	SDL_SetRenderDrawColor(Main::renderer, 0, 0, 0, 255);
	for (auto& bound : boundsArr) {
		FVector2& min = std::get<0>(bound), & max = std::get<1>(bound);
		SDL_RenderDrawLineF(Main::renderer, min.x, min.y, max.x, max.y);
	}
	boundsArr.clear();
	SDL_SetRenderDrawColor(Main::renderer, std::get<0>(Main::renderDrawColor), std::get<1>(Main::renderDrawColor), std::get<2>(Main::renderDrawColor), 255);
#endif
}
void Physics::Finalize() {
	curNode = entityHead;
	threadIndex = 0;
	rbList* nextNode;
	while (curNode) {
		nextNode = curNode->GetNext();
		Physics::DeleteRB(curNode);
		curNode = nextNode;
	}
#ifdef IS_MULTI_THREADED
	for (int i = 0; i < thread_count; i++) {
		{
			std::unique_lock<std::mutex> lock(threadFuncMutexes[i]);
			stopThread[i] = true;
		}
		threadFuncConds[i].notify_one();
//#define MAKE_JOIN_MESSAGE
#ifdef MAKE_JOIN_MESSAGE
		cout << "trying to join threads...\n";
#endif
		workers[i].join();
#ifdef MAKE_JOIN_MESSAGE
		cout << "done\n";
		char* line = new char[MAX_PATH];
		sprintf(line, "%d", __LINE__); cout << (string("to get rid of this text and the \"triyng to join threads... done\" text, goto ") + __FILE__ + ": " + line + '\n').c_str();//all on the same line so there is no offset between the line printed and where the original source code lies.
		delete[] line;
#endif
	}
#endif
}