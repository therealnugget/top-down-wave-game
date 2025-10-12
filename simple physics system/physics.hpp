#pragma once
#include "usefulTypedefs.hpp"
#include <tuple>
#include "textures.hpp"
#include <SDL.h>
#include "main.hpp"
#include "debug.hpp"
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <unordered_map>
#include <variant>
#include <unordered_set>
#define DEFAULT_SQUARE_POINTS {-.5f, -.5f},\
{ -.5f, .5f },\
{ .5f, .5f },\
{ .5f, -.5f }
#define IS_MULTI_THREADED
using namespace std;
struct RigidBody;
//every instantiation of this *should* always be 8 bytes in total
template<typename T>
struct Vector2 {
public:
	T x, y;
	constexpr Vector2(T _x, T _y) : x(static_cast<T>(_x)), y(static_cast<T>(_y)) {}
	Vector2() : x(.0f), y(.0f) {}
	Vector2(SDL_DisplayMode &mode) : x(mode.w), y(mode.h) {}
#ifdef DEBUG_BUILD
	inline Vector2 &PrintVec() {
		cout << x << ", " << y << '\n';
		return *this;
	}
#endif
	inline explicit operator bool() const {
		return x || y;
	}
	inline explicit operator Vector2<int>() const {
		return Vector2<int>(static_cast<int>(x), static_cast<int>(y));
	}
	inline Vector2 operator -(Vector2 b) {
		return Vector2(x - b.x, y - b.y);
	}
	inline Vector2 operator -(float b) {
		return Vector2(x - b, y - b);
	}
	inline Vector2 operator +(Vector2 b) {
		return Vector2(x + b.x, y + b.y);
	}
	inline Vector2 operator +=(Vector2 b) {
		return *this = *this + b;
	}
	inline Vector2 operator -=(Vector2 b) {
		return *this = *this - b;
	}
	inline Vector2 operator -=(float b) {
		return *this = *this - b;
	}
	inline Vector2 operator /=(Vector2 b) {
		return *this = *this / b;
	}
	inline Vector2 operator /=(T b) {
		return *this = *this / b;
	}
	inline Vector2 operator -() {
		return Vector2(-x, -y);
	}
	inline static Vector2 constexpr ConstNeg(const FVector2 &vec) {
		return Vector2(-vec.x, -vec.y);
	}
	inline Vector2 operator /(Vector2 b) {
		return Vector2(x / b.x, y / b.y);
	}
	inline Vector2 operator /(float b) {
		return Vector2(x / b, y / b);
	}
	inline Vector2 operator *(float b) {
		return Vector2(x * b, y * b);
	}
	inline static Vector2 constexpr ConstMult(const FVector2 &vec, float b) {
		return Vector2(vec.x * b, vec.y * b);
	}
	inline Vector2 operator *=(Vector2 b) {
		return *this = *this * b;
	}
	inline Vector2 operator *=(float b) {
		return *this = *this * b;
	}
	//NOT the cross product.
	inline Vector2 operator *(Vector2 b) {
		return Vector2(x * b.x, y * b.y);
	}
	//NOT the cross product.
	inline std::vector<Vector2> operator *(std::vector<Vector2> list) {
		for (auto& vec : list) {
			vec *= *this;
		}
		return list;
	}
	inline bool operator ==(Vector2 b) {
		return x == b.x && y == b.y;
	}
	inline float Magnitude() {
		return sqrtf(static_cast<float>(Sqr(x) + Sqr(y)));
	}
	inline void IntoRectXY(SDL_Rect *rect) {
		rect->x = x;
		rect->y = y;
	}
	inline void IntoRectWH(SDL_Rect* rect) {
		rect->w = x;
		rect->h = y;
	}
	//dot product (carret is more readable)
	inline float operator ^(Vector2 &b) {
		return x * b.x + y * b.y;
	}
	inline Vector2 Normalized() {
		T r = Magnitude();
		if (!r) return Zero;
		return Vector2(*this / r);
	}
	inline static Vector2 FromTo(FVector2 from, FVector2 to) {
		return -from + to;
	}
	static Vector2<T> FromTo(RigidBody *from, RigidBody *to);
	inline void Normalize() {
		*this = Normalized();
	}
	static const Vector2 One;
	static const Vector2 Zero;
	static const Vector2 Left;
	static const Vector2 Right;
	static const Vector2 Down;
	static const Vector2 Up;
	//non-const equivalents of the vector primitives
	inline static Vector2 GetOne(){
		return One;
	}
	inline static Vector2 GetZero(){
		return Zero;
	}
	inline static Vector2 GetLeft(){
		return Left;
	}
	inline static Vector2 GetRight(){
		return Right;
	}
	inline static Vector2 GetDown(){
		return Down;
	}
	inline static Vector2 GetUp() {
		return Up;
	}
	inline static const Vector2 DirToVec(Main::direction dir) {
		switch (dir) {
		case Main::down:
			return Down;
		case Main::up:
			return Up;
		case Main::left:
			return Left;
		case Main::right:
			return Right;
		}
	}
	inline static Vector2 GetDirToVec(Main::direction dir) {
		return const_cast<Vector2>(DirToVec(dir));
	}
	//MUST BE STACK FREED AFTER USE. repeats horizontal across horizontal directions, vertical across vertical directions
	inline static Vector2<T>* GetRepeatingVec(Vector2<T> h, Vector2<T> v) {
		Vector2<T>* _this = static_cast<Vector2<T> *>(_malloca(sizeof(Vector2<T>) * Main::num_directions));
		_this[Main::right] = h;
		_this[Main::left] = h;
		_this[Main::up] = v;
		_this[Main::down] = v;
		return _this;
	}
	//the decision between whether you want a per-direction vector size or the same size across all of the directions for that animation
	union VecDirSize {
		Vector2<T> globalSize;
		Vector2<T>* dirSizes;
	};
private:
	inline T Sqr(T n) {
		return n * n;
	}
};
typedef Vector2<float> FVector2;
typedef Vector2<int> IntVec2;
typedef tuple<int, int> IntVec2Tuple;
template<>
struct Vector2<int> {
public:
	int x, y;
	Vector2(int _x, int _y) : x(_x), y(_y) {}
	Vector2(FVector2 v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)) {}
	Vector2() : x(.0f), y(.0f) {}
#ifdef DEBUG_BUILD
	inline void PrintVec() {
		cout << x << ", " << y << '\n';
	}
#endif
	inline Vector2 operator +(Vector2 b) {
		return Vector2(x + b.x, y + b.y);
	}
	inline Vector2 operator +=(Vector2 b) {
		return *this = *this + b;
	}
	inline Vector2 operator /=(Vector2 b) {
		return *this = *this / b;
	}
	inline Vector2 operator /=(int b) {
		return *this = *this / b;
	}
	inline Vector2 operator -() {
		return Vector2(-x, -y);
	}
	inline Vector2 operator /(Vector2 b) {
		return Vector2(x / b.x, y / b.y);
	}
	inline operator FVector2() {
		return FVector2(static_cast<float>(x), static_cast<float>(y));
	}
	//division of int vector2s is VERY slow for a division, because there are TWO casts that need to occur (technically 4, as one is a user-defined cast of a struct with two primitives that also must be casted.) for this reason, if you want this division to happen within a reasonable amount of time and know that the integer conversion won't fuck up (i.e. truncate) the coords of the resultant vector, divide the components directly.
	inline Vector2 operator /(int b) {
		return Vector2(static_cast<FVector2>(*this) / static_cast<float>(b));
	}
	inline Vector2 operator *(Vector2 b) {
		return Vector2(x * b.x, y * b.y);
	}
	inline Vector2 operator *(int b) {
		return Vector2(x * b, y * b);
	}
	inline float Magnitude() {
		return sqrtf(Sqr(x) + Sqr(y));
	}
	//dot product (carret is more readable)
	inline float operator ^(Vector2 b) {
		return x * b.x + y * b.y;
	}
	inline Vector2 Normalized() {
		float r = Magnitude();
		if (!r) goto ret;
		*this /= r;
	ret:
		return *this;
	}
	inline void IntoRectWH(SDL_Rect* rect) {
		rect->w = x;
		rect->h = y;
	}
	static const Vector2 One;
	static const Vector2 Zero;
	static const Vector2 Left;
	static const Vector2 Right;
	static const Vector2 Down;
	static const Vector2 Up;
	//non-const equivalents of the vector primitives
	inline static Vector2 GetOne() {
		return One;
	}
	inline static Vector2 GetZero() {
		return Zero;
	}
	inline static Vector2 GetLeft() {
		return Left;
	}
	inline static Vector2 GetRight() {
		return Right;
	}
	inline static Vector2 GetDown() {
		return Down;
	}
	inline static Vector2 GetUp() {
		return Up;
	}
	//y is reversed
	inline static const Vector2 DirToVec(Main::direction dir) {
		switch (dir) {
		case Main::down:
			return Up;
		case Main::up:
			return Down;
		case Main::left:
			return Left;
		case Main::right:
			return Right;
		}
	}
	//y is reversed
	inline static const int VecToDir(Vector2 vec) {
		switch (vec.x) {
		case 1:
			return Main::right;
		case -1:
			return Main::left;
		}
		switch (vec.y) {
		case 1:
			return Main::down;
		case -1:
			return Main::up;
		}
	}
private:
	inline int Sqr(int n) {
		return n * n;
	}
};
//maybe only use this as a reference or ptr
struct Entity : public Animator {
private:
	SDL_Texture* texture;
	float angle;
	const SDL_RendererFlip flip;
	SDL_Point* centreOfRotation;
public:
	//last boolean argument of image sizes is whether you're using the per-component size
	//would be more efficient to have "image sizes" and "is global sizes" as one map, but it is more readable & maintainable to separate them into two.
	Entity(float _angle, const std::string &basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*> &dirPaths, IntVec2 size, std::unordered_map<const char *, std::variant<FVector2, FVector2*>> _imageSizes, std::unordered_map<const char *, bool> _isGlobalSize, IntVec2 _renderOffset) : angle(_angle), flip(SDL_FLIP_NONE), renderOffset(_renderOffset) {
		centreOfRotation = new SDL_Point;
		rect = new SDL_Rect;
		size.IntoRectWH(rect);
#ifdef DEBUG_BUILD
		if (dirPaths.size() == 0) {
			ThrowError(4, "\"end paths\"'s size is 0 at ", to_string(__LINE__).c_str(), " in ", __FILE__);
		}
#endif
		int i = 0, j;
		const auto animPathsSize = animPaths.size();
		const auto FVecSize = static_cast<FVector2>(size);
		for (auto& animPath : animPaths) {
			j = 0;
			for (auto& path : dirPaths) {
				Textures::InitAnim(*this, (basePath + animPath + '_' + path).c_str());
				if (i == 0 && j == 0) SetTexture(anims[0].textures[0]);
				if (_imageSizes.size() == 0) {
					continue;
				}
				if (i == 0 && j == 0) {
					imageSizes = new std::variant<IntVec2, IntVec2*>[animPathsSize]();
					isGlobalSize = new bool[animPathsSize];
				}
				auto& curThisImgSize = imageSizes[i];
				const auto& currentIsGlobalSize = _isGlobalSize[animPath];
				isGlobalSize[i] = currentIsGlobalSize;
				const auto& curImgSize = _imageSizes[animPath];
				if (currentIsGlobalSize) {
					if (j > 0) continue;
					curThisImgSize = static_cast<IntVec2>(static_cast<FVector2>(std::get<FVector2>(curImgSize)) * FVecSize);
					continue;
				}
				if (j == 0) curThisImgSize = std::variant<IntVec2, IntVec2*>(new IntVec2[Main::num_directions]);
				static_cast<IntVec2*>(std::get<IntVec2*>(curThisImgSize))[j] = static_cast<FVector2>(static_cast<FVector2*>(std::get<FVector2*>(curImgSize))[j]) * FVecSize;
				j++;
			}
			i++;
		}
	}
	~Entity() {
		int j;
		Animation& curAnim = anims[0];
		bool imageSizesEmpty = !imageSizes;
		for (int i = 0; i < numAnims; i++) {
			curAnim = anims[i];
			for (j = 0; j < curAnim.numOfFrames; j++) {
				SDL_DestroyTexture(curAnim.textures[j]);
			}
			if (imageSizesEmpty) continue;
			delete std::get<IntVec2*>(imageSizes[i]);
		}
		delete imageSizes;
		delete centreOfRotation;
		delete rect;
	}
	inline void SetTexture(SDL_Texture* tex) {
		texture = tex;
	}
	inline SDL_Texture* GetTexture() {
		return texture;
	}/*
	inline void SetNextAnimTex() {
		Animation& curAnim = anims[currentAnimation];
		SetTexture(curAnim.textures[(animFrameIndex = ((animFrameIndex + 1) % curAnim.numOfFrames))]);
	}*/
	inline void SetNextAnimTex(int inc) {
		Animation& curAnim = anims[currentAnimation];
		SetTexture(curAnim.textures[(animFrameIndex = ((animFrameIndex + inc) % curAnim.numOfFrames))]);
	}
	const inline int GetNumAnimFrames(int animation) const {
		return anims[animation].numOfFrames;
	}
	inline IntVec2 GetSize() const {
		return { rect->w, rect->h };
	}
private:
	SDL_Rect* rect;
	//per-direction.
	std::variant<IntVec2, IntVec2*>* imageSizes = nullptr;
	bool* isGlobalSize;
	IntVec2 renderOffset;
	friend class Physics;
	friend class RigidBody;
};
typedef struct AABB {
public:
	constexpr AABB(FVector2 _min, FVector2 _max): minimum(_min), maximum(_max) {}
	AABB(float minX, float minY, float maxX, float maxY) {
		minimum.x = minX;
		minimum.y = minY;
		maximum.x = maxX;
		maximum.y = maxY;
	}
	constexpr AABB() : minimum(FVector2::Zero), maximum(FVector2::Zero) {}
	Vector2<float> minimum, maximum;
};
class Collision {
private:
	RigidBody* collider;
	float dot;
	bool isTriggerCollision;
public:
	Collision(RigidBody* rb, float _dot, bool _isTrigger) : collider(rb), dot(_dot), isTriggerCollision(_isTrigger) {}
	inline void SetCollider(RigidBody* newCol) {
		collider = newCol;
	}
	inline RigidBody* GetCollider() {
		return collider;
	}
	inline float GetDot() {
		return dot;
	}
};
struct RigidBody {
public:
	//vertices of collider are at entity pos at origin.
	RigidBody(FVector2 _position, FVector2 _velocity, float _angle, float _mass, std::vector<FVector2> _narrowPhaseVertices, FVector2 _centreOfRotForNarrowPVert = FVector2::Zero, bool _moveable = true, bool _isTrigger = false, bool createEntity = true, IntVec2 size = IntVec2::One, const std::string& basePath = Main::empty_string, const std::initializer_list<const char*>& animPaths = std::initializer_list<const char *>(), const std::initializer_list<const char*>& endPaths = std::initializer_list<const char *>(), std::unordered_map<const char*, std::variant<FVector2, FVector2*>> imageSizes = std::unordered_map<const char*, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool> isGlobalSize = std::unordered_map<const char*, bool>(), std::initializer_list<FVector2> _centreOfRotation = std::initializer_list<FVector2>(), IntVec2 _renderOffset = IntVec2::Zero, int _tag = -1) : mass(_mass), invMass(1.f / _mass), velocity(_velocity), rotation(.0), numNarrowPhaseVertices(_narrowPhaseVertices.size()), origNarrowPVertices(new FVector2[numNarrowPhaseVertices]), verticesNarrowP(new FVector2[numNarrowPhaseVertices]), 
#ifdef DEBUG_BUILD
		isDebugSquare(false),
#endif
		centreOfNarrowPVertRot(_centreOfRotForNarrowPVert), madeAABBTrue(false), isColliding(false), position(_position), pastPosition(_position), bMoveable(_moveable), bIsTrigger(_isTrigger), OnCollision(nullptr), tag(_tag) {
		if (createEntity) {
			entity = new Entity(_angle, basePath, animPaths, endPaths, size, imageSizes, isGlobalSize, _renderOffset);
			position.IntoRectXY(entity->rect);
			if (_centreOfRotation.size()) {
				memset(entity->centreOfRotation, 0, sizeof(SDL_Point));
			}
			else {
				entity->centreOfRotation->x = size.x / 2;
				entity->centreOfRotation->y = size.y / 2;
			}
		}
		else entity = nullptr;
		SetInitCOR();
		std::copy(_narrowPhaseVertices.begin(), _narrowPhaseVertices.end(), origNarrowPVertices);
#ifdef DEBUG_BUILD
		constexpr int numVertInBox = 4;
		if (numNarrowPhaseVertices < numVertInBox) {
			ThrowError((string("rigidbody was submitted which has less than ") + Main::IntToStr(numVertInBox) + " vertices on line " + Main::IntToStr(__LINE__) + " in file " + __FILE__ + ". size is " + Main::IntToStr(numNarrowPhaseVertices) + '.').c_str());
		}
#endif
	}
	~RigidBody() {
		delete origNarrowPVertices;
		delete verticesNarrowP;
	}
	uint entityIndex;//therefore there can be a max of 2^32-1 entities in the scene. not that the system can handle anywhere close to that, of course.
	int tag;
#ifdef DEBUG_BUILD
	bool isDebugSquare;
#endif
	inline float GetMass() {
		return mass;
	}
	inline float GetInvMass() {
		return invMass;
	}
	inline void AddForce(FVector2 f) {
		force += f;
		madeAABBTrue = f != FVector2::Zero;
	}
	inline FVector2 GetVelocity() const {
		return velocity;
	}
	inline void SetVelocity(FVector2 value) {
		if (velocity == value) return;
		madeAABBTrue = false;
		velocity = value;
	}
	inline void SetPosition(FVector2 value) {
		if (position == value) return;
		position = value;
		madeAABBTrue = false;
	}
	inline FVector2 GetPosition() {
		return position;
	}
	inline AABB &GetBroadPAABB() {
		return broadPhaseAABB;
	}
	inline FVector2* GetNarrowPhaseVertices() {
		return verticesNarrowP;
	}
	inline FVector2* NarrowPAtI(int i) {
		return verticesNarrowP + i;
	}
	inline uint GetNumNarrowPhaseVertices() {
		return numNarrowPhaseVertices;
	};
	inline void SetPosition(float x, float y) {
		position.x = x;
		position.y = y;
		madeAABBTrue = false;
	}
	inline void SetPosition(FVector2 &pos) {
		SetPosition(pos.x, pos.y);
	}
private:
	std::function<void(Collision&)> OnCollision;
	bool bMoveable;
	bool bIsTrigger;
	FVector2 difPositionSection;
	std::atomic<bool> isColliding;
	std::condition_variable collidingCond;
	std::mutex checkColliding;
	const uint numNarrowPhaseVertices;
	FVector2* origNarrowPVertices;
	void SetInitCOR();
	float coefRestitution;
	//centre of rotation for the narrow-phase vertices.
	FVector2 centreOfNarrowPVertRot;
	double rotation;
	inline FVector2* GetOrigNarrowPVertices() {
		return origNarrowPVertices;
	}
	FVector2 velocity;
	bool madeAABBTrue;
	//broad-phase
	AABB broadPhaseAABB;
	//narrow-phase
	FVector2 *verticesNarrowP;
	float mass, invMass;
	FVector2 force;
	Entity *entity;
public:
	inline void SetSize(IntVec2 size) {
		madeAABBTrue &= (FVector2(entity->rect->w, entity->rect->h) == size);
		size.IntoRectWH(entity->rect);
	}
	inline Entity *GetEntity() {
		return entity;
	}
	//get coefficient of restitution
	inline float GetCOR() {
		return coefRestitution;
	}
	inline void SetRotation(double value) {
		madeAABBTrue &= value == rotation;
		rotation = value;
	}
	inline double GetRotation() {
		return rotation;
	}
	//gets the centre of rotation for the narrow-phase collision's position vertices.
	inline FVector2 GetCentreNarrowPVertRot() {
		return centreOfNarrowPVertRot;
	}
	inline bool GetMadeAABBTrue() {
		return madeAABBTrue;
	}
	inline void SetCollisionCallback(std::function<void(Collision&)> callback) {
		OnCollision = callback;
	}
	FVector2 position;
	FVector2 pastPosition;
	friend class Physics;
};
//static
template<typename T>
struct QuadNode;
class Physics {
private:
	//decreasing the number of movement iterations increases fps at the expense of physical accuracy
	static constexpr int num_movement_iterations = 20;
	static constexpr float inv_num_movement_iterations_f = 1.f / static_cast<float>(num_movement_iterations);
	static constexpr FVector2 initCellSize = { 10000.f, 10000.f };
	static constexpr AABB initCellAABB = { FVector2::ConstNeg(FVector2::ConstMult(initCellSize, .5f)), FVector2::ConstMult(initCellSize, .5f) };
	static constexpr int thread_count = 10;
	static constexpr float thread_count_f = static_cast<float>(thread_count);
	static const float frictionMagnitude;
	static const float fricCoef;
	static int threadIndex;
	static rbList *entityHead;
	static rbListList *sortedEntityHeads;
	static rbListList *unsortedEntityHeads;
	static QuadNode<RigidBody*> quadRoot;
	static int numEntities;
	static int numSections;
	static int sectionSize;
	static int excessEntityNo;
	static int moveItrIndex;
	static const FVector2 frictionVec;
	//a non-constant alternative to frictionVec. don't calculate this magnitude, a compile-time constant already contains it.
	static FVector2 GetFrictionVec() {
		return frictionVec;
	}
	static inline FVector2 GetInitCellSize() {
		return initCellSize;
	}
	static std::unordered_set<uint_fast64_t> entitiesCollided;
	static inline uint_fast64_t GetDoubleEntColIndex(uint_fast64_t aIndex, uint_fast64_t bIndex) {
		return aIndex | (bIndex << 32);
	}
	static void UnSubscribeEntity(rbList*);
public:
	static inline void SetEntitiesCollided(uint_fast64_t collisionIndex) {
		entitiesCollided.emplace(collisionIndex);
	}
	static inline bool GetEntityCollided(uint aIndex, uint bIndex, uint_fast64_t * collisionIndex) {
		auto GetEntCollided = [collisionIndex](uint a, uint b) -> bool {
			*collisionIndex = GetDoubleEntColIndex(static_cast<uint_fast64_t>(a), static_cast<uint_fast64_t>(b));
			return entitiesCollided.contains(*collisionIndex);
			};
		return GetEntCollided(aIndex, bIndex) || GetEntCollided(bIndex, aIndex);
	}
	static inline void ClearCollidedEntites() {
		//if (entitiesCollided.size() == 0) return;
		entitiesCollided.clear();
	}
	static inline rbList* GetEntHead() {
		return entityHead;
	}
	static Node<RigidBody*>* SubscribeEntity(const std::string &basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*> &texturePath, std::vector<FVector2> narrowPhaseVertices = Physics::DefaultSquareVerticesAsList, FVector2 startPos = FVector2::Zero, IntVec2 size = IntVec2::One, std::initializer_list<FVector2> _centreOfRot = std::initializer_list<FVector2>(), FVector2 _centreOfRotNPVert = FVector2::Zero, IntVec2 _renderOffset = IntVec2::Zero, int tag = -1, std::unordered_map<const char*, std::variant<FVector2, FVector2 *>> imageSizes = std::unordered_map<const char *, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool> isGlobalSize = std::unordered_map<const char *, bool>(), FVector2 initVel = FVector2::Zero, float angle = .0f, float mass = 1.f, bool moveable = true, bool isTrigger = false);
	static Node<RigidBody*> *SubscribeEntity(RigidBody *);
	static void DeleteRB(rbList*);
	static Node<RigidBody*>* StandaloneRB(FVector2 size = FVector2::One, FVector2 startPos = FVector2::Zero, bool isTrigger = true, float mass = 1.f, bool moveable = true, FVector2 _centreOfRotNPVert = FVector2::Zero, FVector2 initVel = FVector2::Zero, float angle = .0f, int tag = -1);
	static void Finalize();
	static void Update(float dt);
	static void OuterBroadPhase(bool searchSorted = false);
	static void SortEntity(QuadNode<RigidBody *>*, Node<RigidBody*>* entities, int currentDepth = 0);
	static void DeleteQuadEntities(QuadNode<RigidBody*>*, bool isRoot = false);
	static void BroadPhase(Node<RigidBody *> *rb
#ifdef IS_MULTI_THREADED
		, int index
#endif
	);
	static void NarrowPhase(RigidBody *, RigidBody *
#ifdef IS_MULTI_THREADED
		, int index
#endif
	);
	static void ProjectVertices(FVector2* vertices, uint numVertices, FVector2 axis, float &min, float &max);
	static void AdjustColVertices(RigidBody *rb, bool addPos = true);
	static void ThreadFunc(int);
	static void Init();
	//this works because only exactly straight lines are drawn from vertex-to-vertex, thus the greatest and smallest points of each entity must be a vertex.
	static inline bool EntityInBoxBroadPhase(AABB box, RigidBody* rb) {
		AABB& Bp = rb->GetBroadPAABB();
		return (Bp.minimum.x >= box.minimum.x || Bp.maximum.x >= box.minimum.x) && (Bp.minimum.x <= box.maximum.x || Bp.maximum.x <= box.maximum.x) && (Bp.minimum.y >= box.minimum.y || Bp.maximum.y >= box.minimum.y) && (Bp.minimum.y <= box.maximum.y || Bp.maximum.y <= box.maximum.y);
	}
	static std::condition_variable threadFuncConds[Physics::thread_count];
	static std::condition_variable mainWaitConds[Physics::thread_count];
	static std::atomic<bool> canRunThread[Physics::thread_count];
	static bool stopThread[Physics::thread_count];
	static std::mutex threadFuncMutexes[Physics::thread_count];
	static std::mutex mainWaitMutexes[Physics::thread_count];
	static std::thread workers[Physics::thread_count];
	//collision events are thread-safe
	/*
	from 3 iterations of tests i ran for respectively dict sizes of 10, 1000000, and 10000000, the time taken increased exponentially, and the practical scenarios were also worse for trying to use a key to fnid a value that we knew didn't exist in the dictionary. the results are shown below.
	* "find()" function for all values known:
		average:
	unordered_map size:	10			3.00E-06
						|			|
						|*100000	|   * 8.61E+04
						|			|
					   \/		   \/
						1000000		0.258187
						|			|
						|*10		|   * 11.64668761
						|			|   (expected: 8.61) (+3.04 increase)
						\/			\/
						10000000	3.007023333
	* "find()" function for all values unknown:
		average:
	unordered_map size:	10			2.33E-06
						|			|
						|*100000	|   * 1.12E+05
						|			|
					   \/		   \/
						1000000		0.261699
						|			|
						|*10		|   * 21.25099192
						|			|   (expected: 11.2) (+10.05 increase)
					   \/		   \/
						10000000	5.561363333


										increase from values that did exist to values that didn't (10.05 - 3.04):
										+7.01 seconds.


	*/
	/*
	struct DoubleRB {
		DoubleRB(RigidBody *_a, RigidBody *_b): a(_a), b(_b) {}
		RigidBody* a, * b;
		bool operator == (const DoubleRB & other) const {
			return a == other.a && b == other.b || a == other.b && b == other.a;
		}
	};*/
	static const std::initializer_list<FVector2> DefaultSquareVerticesAsList;
	static inline std::initializer_list<FVector2> GetDefaultSquareVertList() {
		return DefaultSquareVerticesAsList;
	}
	static inline std::vector<FVector2> GetDefaultSquareVertVec() {
		return std::vector<FVector2>(DefaultSquareVerticesAsList);
	}
	static constexpr int numVerticesInSquare = 4;
	static constexpr FVector2 DefaultSquareVertices[numVerticesInSquare] = {
		DEFAULT_SQUARE_POINTS,
	};
	static constexpr int max_quadtree_depth = 8;
	static constexpr int max_ent_per_cell = 5;
	static constexpr float deg2rad = 3.1415926535897932384626433832795028841971693993751058209749445923 / 180.0;
	static constexpr float defaultCOR = .4f;
};
//#define SHOW_QUAD_TREE
#ifdef SHOW_QUAD_TREE
static std::vector<std::tuple<FVector2, FVector2>> boundsArr;
#endif
template<typename T>
struct QuadNode {
private:
	QuadNode* topLeft, * topRight, * bottomLeft, * bottomRight;
	AABB aabb;
public:
	typedef enum TypeOfNode {
		tTopLeft,
		tTopRight,
		tBottomLeft,
		tBottomRight,
		numOfNodes,
		all,
	};
	static constexpr uint numNodes = numOfNodes;
	inline QuadNode(FVector2 &AABBMin, FVector2 &AABBMax) : topLeft(nullptr), topRight(nullptr), bottomLeft(nullptr), bottomRight(nullptr), aabb(AABBMin, AABBMax), values(nullptr) {
	}
	inline QuadNode(float minX, float minY, float maxX, float maxY) : topLeft(nullptr), topRight(nullptr), bottomLeft(nullptr), bottomRight(nullptr), aabb(minX, minY, maxX, maxY), values(nullptr) {
	}
	inline QuadNode(const AABB& _aabb) : topLeft(nullptr), topRight(nullptr), bottomLeft(nullptr), bottomRight(nullptr), aabb(_aabb), values(nullptr) {
	}
	inline AABB GetAABB() {
		return aabb;
	}
	Node<T>* values;
public:
	inline QuadNode* GetBottomLeft() {
		return bottomLeft;
	}
	inline QuadNode* GetBottomRight() {
		return bottomRight;
	}
	inline QuadNode* GetTopLeft() {
		return topLeft;
	}
	inline QuadNode** GetTopLeftAddr() {
		return &topLeft;
	}
	inline QuadNode* GetTopRight() {
		return topRight;
	}
	inline bool IsLeafNode() {
		return topLeft == nullptr;
	};
	inline void Enact(std::function<void(Node<T>*, QuadNode<T>*)> del, TypeOfNode typeOfNode) {
		QuadNode* node = nullptr;
		if (IsLeafNode()) CreateChildNodes();
		switch (typeOfNode) {
		case TypeOfNode::tTopLeft:
			node = topLeft;
			break;
		case TypeOfNode::tTopRight:
			node = topRight;
			break;
		case TypeOfNode::tBottomLeft:
			node = bottomLeft;
			break;
		case TypeOfNode::tBottomRight:
			node = bottomRight;
			break;
		case TypeOfNode::all:
			Enact(del, TypeOfNode::tBottomLeft);
			Enact(del, TypeOfNode::tBottomRight);
			Enact(del, TypeOfNode::tTopLeft);
			Enact(del, TypeOfNode::tTopRight);
			return;
			//this redundant label does not hurt performance, since the other labels in the switch statement actually do something, thus the switch statement is not useless, and this label will never be jumped to so it won't detriment performance.
		default:
			ThrowError("case not defined");
			break;
		}
		del(values, node);
	}
	inline void CreateChildNodes() {
		FVector2 min, max;
		FVector2 halfAABB = (aabb.minimum + aabb.maximum) * .5f;
		FVector2& minAABB = aabb.minimum, & maxAABB = aabb.maximum;
		for (unsigned int i = 0; i < numNodes; i++) {
			switch (i) {
			case TypeOfNode::tBottomLeft:
				bottomLeft = new QuadNode(minAABB, halfAABB);
				continue;
			case TypeOfNode::tBottomRight:
				bottomRight = new QuadNode(halfAABB.x, minAABB.y, maxAABB.x, halfAABB.y);
				continue;
			case TypeOfNode::tTopLeft:
				topLeft = new QuadNode(minAABB.x, halfAABB.y, halfAABB.x, maxAABB.y);
				continue;
			case TypeOfNode::tTopRight:
				topRight = new QuadNode(halfAABB, maxAABB);
				continue;
			default:
				throw new std::exception("'i' is not implemented for this value\n");
			}
		}
		}
};