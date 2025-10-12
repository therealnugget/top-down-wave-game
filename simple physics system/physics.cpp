#include "physics.hpp"
#include "main.hpp"
#include "debug.hpp"
#include "math.hpp"
#include <limits>
rbList *Physics::entityHead = nullptr;
rbListList *Physics::sortedEntityHeads = nullptr;
rbListList *Physics::unsortedEntityHeads = nullptr;
//TODO (minor): rename the following 4 variables
int Physics::numEntities = 0;
int Physics::sectionSize = 0;
int Physics::numSections = 0;
int Physics::excessEntityNo = 0;
std::unordered_set<uint_fast64_t> Physics::entitiesCollided = std::unordered_set<uint_fast64_t>();
const std::initializer_list<FVector2> Physics::DefaultSquareVerticesAsList = {
	{ -.5f, -.5f },{ -.5f, .5f },{ .5f, .5f },{ .5f, -.5f },
};
Node<RigidBody*>* Physics::SubscribeEntity(const std::string &basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*> &endPaths, std::vector<FVector2> _narrowPhaseVertices, FVector2 startPos, IntVec2 size, std::initializer_list<FVector2> _centreOfRot, FVector2 _centreOfRotNPVert, IntVec2 _renderOffset, int tag, std::unordered_map<const char*, std::variant<FVector2, FVector2*>> imageSizes, std::unordered_map<const char *, bool> isGlobalSize, FVector2 initVel, float angle, float mass, bool moveable, bool isTrigger) {
	RigidBody *rb = new RigidBody(startPos, initVel, angle, mass, _narrowPhaseVertices, _centreOfRotNPVert, moveable, isTrigger, true, size, basePath, animPaths, endPaths, imageSizes, isGlobalSize, _centreOfRot, _renderOffset, tag);
	return SubscribeEntity(rb);
}
Node<RigidBody*> *Physics::SubscribeEntity (RigidBody *rb){
	static uint totalNumEntities = 0;
	rb->entityIndex = totalNumEntities++;
	return Node<RigidBody*>::AddAtHead(rb, &entityHead);
}
void Physics::UnSubscribeEntity(rbList* node) {
	rbList::Remove(&entityHead, node);
}
void Physics::DeleteRB(rbList* node) {
	RigidBody* rb = node->value;
	Entity* ent = rb->GetEntity();
	if (ent) delete ent;
	delete rb;
	UnSubscribeEntity(node);
}
Node<RigidBody*>* Physics::StandaloneRB(FVector2 size, FVector2 startPos, bool isTrigger, float mass, bool moveable, FVector2 _centreOfRotNPVert, FVector2 initVel, float angle, int tag) {
	RigidBody* rb = new RigidBody(startPos, initVel, angle, mass, size * Physics::GetDefaultSquareVertVec(), _centreOfRotNPVert, moveable, isTrigger, false);
	rb->tag = tag;
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
	/*
	for (auto& rb : rbs) {
		printf("%d\n", rb->entityIndex);
	}*/
	{
		std::lock_guard<std::mutex> lockOuterMutex(outerMutex);
		uint_fast64_t collisionIndex;
		if (GetEntityCollided(a->entityIndex, b->entityIndex, &collisionIndex)) return;
		SetEntitiesCollided(collisionIndex);
		a->checkColliding.lock();
		b->checkColliding.lock();
	}
	/*
	auto waitTilNotCol = [&](RigidBody* rb) {
		std::unique_lock<std::mutex> lock(rb->checkColliding);
		while (rb->isColliding
#ifdef IS_MULTI_THREADED
			&& !stopThread[index]
#endif
			) {
			rb->collidingCond.wait(lock);
		}
		};
	for (auto& rb : rbs) {
		waitTilNotCol(rb);
	}*/
	{
		int i;
		float minA, maxA;
		float minB, maxB;
		bool colliding = true;
		FVector2 vec;
		FVector2 vecB, edge, axis;
		float axisDepth;
		auto checkVertices = [&](RigidBody* rb) {
			if (!rb->GetMadeAABBTrue()) {
				Physics::AdjustColVertices(rb);
				rb->madeAABBTrue = true;
			}
			FVector2* vertices = rb->GetNarrowPhaseVertices();
			uint numVertices = rb->GetNumNarrowPhaseVertices();
			for (i = 0; i < numVertices; i++) {
				vecB = vertices[(i + 1) % numVertices];
				edge = vecB - vertices[i];
				axis = FVector2(-edge.y, edge.x).Normalized();
				ProjectVertices(a->GetNarrowPhaseVertices(), a->GetNumNarrowPhaseVertices(), axis, minA, maxA);
				ProjectVertices(b->GetNarrowPhaseVertices(), b->GetNumNarrowPhaseVertices(), axis, minB, maxB);
				colliding &= minA < maxB && minB < maxA;
				axisDepth = Math::Min(maxB - minA, maxA - minB);
				if (axisDepth < depth) {
					depth = axisDepth;
					normal = axis;
				}
			}
			};
		checkVertices(a);
		checkVertices(b);
		FVector2 aPos = a->GetPosition(), bPos = b->GetPosition();
		if ((FVector2::FromTo(aPos, bPos) ^ normal) < .0f) normal *= -1.f;
		bool aMoveable = a->bMoveable, bMoveable = b->bMoveable;
		float dot = .0f;
		bool triggerCollision = a->bIsTrigger || b->bIsTrigger;
		if (!colliding || triggerCollision)
			goto ret;

		{
			FVector2 offset = normal * depth * .5f;
			a->SetPosition(aPos - offset * aMoveable - offset * !bMoveable);
			b->SetPosition(bPos + offset * bMoveable + offset * !aMoveable);
			if (aMoveable) Physics::AdjustColVertices(a);
			if (bMoveable) Physics::AdjustColVertices(b);
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
	ret:;
		if (!colliding || moveItrIndex != 0) {
			goto free_mutex;
		}
		{
			Collision collisionData = Collision(b, dot, triggerCollision);
			if (a->OnCollision) a->OnCollision(collisionData);
			if (!b->OnCollision) goto free_mutex;
			collisionData.SetCollider(a);
			b->OnCollision(collisionData);
		}
	}
	free_mutex:
	a->checkColliding.unlock();
	b->checkColliding.unlock();
	/*
	auto SetNotCol = [&](RigidBody* rb) {
		{
			std::unique_lock<std::mutex> lock(rb->checkColliding);
			rb->isColliding = false;
		}
		rb->collidingCond.notify_one();
		};
	for (auto& rb : rbs) {
		SetNotCol(rb);
	}*/
}
void Physics::ProjectVertices(FVector2 *vertices, uint numVertices, FVector2 axis, float &min, float &max) {
	float projection;
	for (uint i = 0; i < numVertices; i++) {
		projection = vertices[i] ^ axis;
		if (projection < min || i == 0) {
			min = projection;
		}
		if (projection > max || i == 0) {
			max = projection;
		}
	}
}
void Physics::OuterBroadPhase(bool searchSortedList) {
#ifndef IS_MULTI_THREADED
	auto curEntHeadList = searchSortedList ? sortedEntityHeads : unsortedEntityHeads;
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
	int i;
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
		rbListList *curMtList = sortedEntityHeads;
		for (i = 0; i < sectionSize * index; i++) {
			//"go to after."
			//"after what?"
			//"after after."
			if (!curMtList) return;
			rbListList::Advance(&curMtList);
		}
		for (i = 0; i < sectionSize + excessEntityNo * (index == (numSections - 1)); i++) {
			if (!curMtList) return;
			BroadPhase(curMtList->value
#ifdef IS_MULTI_THREADED
				, index
#endif
			);
			rbListList::Advance(&curMtList);
		}
		if (index == 0) {
			curMtList = unsortedEntityHeads;
			while (curMtList) {
				BroadPhase(curMtList->value
#ifdef IS_MULTI_THREADED
					, index
#endif
				);
				rbListList::Advance(&curMtList);
			}
		}
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
static FVector2 toAddToPos;
int Physics::moveItrIndex;
static SDL_Rect* curRect;
static bool lastMovementItr;
QuadNode<RigidBody*> Physics::quadRoot = QuadNode<RigidBody *>(Physics::initCellAABB);
void Physics::DeleteQuadEntities(QuadNode<RigidBody*>* tree, bool isRoot) {
	Node<RigidBody*>::RemoveAllNodes(&tree->values);
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
		return;
	}
	delete tree;
}
void Physics::SortEntity(QuadNode<RigidBody*>* quadNode, Node<RigidBody *> *entities, int currentDepth) {
	int noEntInCurCell = 0;
	RigidBody* curRB;
	rbList* unsortedRbs = nullptr;
	auto AddRbsToNPList = [unsortedRbs](rbList* vals, rbListList *head) -> void {
		rbListList::AddAtHead(vals, &head);
		if (vals == unsortedRbs) return;
		numEntities += vals->Length();
		sectionSize = numEntities / thread_count;
		excessEntityNo = numEntities % thread_count;
		numSections = std::min(thread_count, numEntities);
		};
	while (entities) {
		curRB = entities->value;
		if (Physics::EntityInBoxBroadPhase(quadNode->GetAABB(), curRB)) {
			rbList::AddAtHead(curRB, &quadNode->values);
			noEntInCurCell++;
		}
		else if (currentDepth == 0) rbList::AddAtHead(curRB, &unsortedRbs);
		Node<RigidBody*>::Advance(&entities);
	}
	if (unsortedRbs) AddRbsToNPList(unsortedRbs, unsortedEntityHeads);
	if (noEntInCurCell < Physics::max_ent_per_cell || currentDepth > Physics::max_quadtree_depth) {
		rbList* vals = quadNode->values;
		if (!vals) {
			return;
		}
		AddRbsToNPList(vals, sortedEntityHeads);
		return;
	}
	quadNode->CreateChildNodes();
#ifdef SHOW_QUAD_TREE
	FVector2 mid = (quadNode->GetAABB().minimum + quadNode->GetAABB().maximum) * .5f, min = quadNode->GetAABB().minimum, max = quadNode->GetAABB().maximum;
	boundsArr.push_back({ { mid.x, min.y }, { mid.x, max.y } });
	boundsArr.push_back({ { min.x, mid.y }, { max.x, mid.y } });
#endif
	auto sort = [quadNode, currentDepth](QuadNode<RigidBody*>* curQuadNode) -> void {SortEntity(curQuadNode, quadNode->values, currentDepth + 1); };
	sort(quadNode->GetBottomLeft());
	sort(quadNode->GetBottomRight());
	sort(quadNode->GetTopRight());
	sort(quadNode->GetTopLeft());
}
//slow af. matrix rotation multiplication, then adds the position to the vertices and it's O(n) where n is num vertices for the rb. default is pos and rot, otherwise specified just rot.
void Physics::AdjustColVertices(RigidBody* rb, bool addPos) {
	double sinB, cosB;
	FVector2* curVertex;
	FVector2 currentVertexWithoutPos;
	FVector2 currentCentreOfNPVertRot;
	FVector2 vertexBeforeRotCalc;
	double currentRotation;
	for (uint i = 0; i < rb->GetNumNarrowPhaseVertices(); i++) {
		curVertex = rb->NarrowPAtI(i);
		currentVertexWithoutPos = rb->GetOrigNarrowPVertices()[i];
		if (rb->entity) currentVertexWithoutPos *= rb->entity->GetSize();
		currentRotation = rb->GetRotation() * Physics::deg2rad;
		sinB = std::sin(currentRotation);
		cosB = std::cos(currentRotation);
		currentCentreOfNPVertRot = rb->centreOfNarrowPVertRot;
		vertexBeforeRotCalc = currentVertexWithoutPos - currentCentreOfNPVertRot;
		/*
		(x, y) --> (ycos(B) - ysin(B), xSin(B) + ycos(B))
		*/
		curVertex->x = static_cast<float>(vertexBeforeRotCalc.x * cosB - vertexBeforeRotCalc.y * sinB);
		curVertex->y = static_cast<float>(vertexBeforeRotCalc.x * sinB + vertexBeforeRotCalc.y * cosB);
		*curVertex += currentCentreOfNPVertRot;
		if (!addPos) {
			continue;
		}
		*curVertex += rb->GetPosition();
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
		if (currentRBVel.Magnitude() > (fricCoefByDT = fricCoef * dt)) currentRB->velocity -= Math::SignOrZero(currentRBVel) * fricCoefByDT;
		else currentRB->velocity = FVector2::Zero;
		currentRB->velocity += currentRB->force * currentRB->invMass * dt;
		currentRB->difPositionSection = (currentRB->position - currentRB->pastPosition) * inv_num_movement_iterations_f;
		AdjustColVertices(currentRB);
		Node<RigidBody*>::Advance(&curNode);
	}
	for (moveItrIndex = 0; moveItrIndex < num_movement_iterations; moveItrIndex++) {
		curNode = entityHead;
		while (curNode) {
			currentRB = curNode->value;
			toAddToPos = (currentRB->velocity * dt) * inv_num_movement_iterations_f;
			for (curVertIndex = 0; curVertIndex < currentRB->numNarrowPhaseVertices; curVertIndex++) {
				curNarrowPVert = &currentRB->GetNarrowPhaseVertices()[curVertIndex];
				*curNarrowPVert += currentRB->difPositionSection + toAddToPos;
				if (curVertIndex == 0) {
					minNarrowPhase = *curNarrowPVert;
					maxNarrowPhase = *curNarrowPVert;
					continue;
				}
				Main::AssignIfMore(*curNarrowPVert, maxNarrowPhase);
				Main::AssignIfLess(*curNarrowPVert, minNarrowPhase);
			}
			currentRB->broadPhaseAABB = { minNarrowPhase, maxNarrowPhase };
			currentRB->position += toAddToPos;
			Node<RigidBody*>::Advance(&curNode);
		}
		numEntities = 0;
		sectionSize = 0;
		numSections = 0;
		excessEntityNo = 0;
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
		ClearCollidedEntites();
		DeleteQuadEntities(&quadRoot, true);
		rbListList::RemoveAllNodes(&sortedEntityHeads);
	}
	curNode = entityHead;
	while (curNode) {
		currentRB = curNode->value;
		currentEntity = currentRB->entity;
		if (currentEntity){
			curRect = currentEntity->rect;
			currentRB->position.IntoRectXY(curRect);
			curRect->x += currentEntity->renderOffset.x;
			curRect->y += currentEntity->renderOffset.y;
			//can only render single-threaded. T-T
			animFramesPassed = 0;
			if (currentEntity->animTime <= Animator::neg_anim_time) {
				animFramesPassed -= static_cast<int>(currentEntity->animTime / Animator::default_anim_time);
				currentEntity->animTime += animFramesPassed * Animator::default_anim_time;
			}
			currentEntity->animTime -= Main::DeltaTime();
			if (currentEntity->animTime <= .0f) {
				currentEntity->animTime += Animator::default_anim_time;
				animFramesPassed++;
			}
			if (animFramesPassed && currentEntity->anims.size() > 0) {
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
			SDL_RenderCopyEx(Main::renderer, currentEntity->texture, nullptr, curRect, currentRB->rotation, currentEntity->centreOfRotation, currentEntity->flip);
		}
		currentRB->force = FVector2::Zero;
		currentRB->pastPosition = currentRB->position;
		Node<RigidBody*>::Advance(&curNode);
		if (!currentEntity || !currentEntity->bRecordAnim) continue;
		currentEntity->pastAnimation = currentEntity->currentAnimation;
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