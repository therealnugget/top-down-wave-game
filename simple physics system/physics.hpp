#pragma once
#include "usefulTypedefs.hpp"
#include <tuple>
#include "textures.hpp"
#include <SDL.h>
#include "linkedList.hpp"
#include "main.hpp"
#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <unordered_map>
#include <variant>
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
	inline std::initializer_list<Vector2> operator *(const std::initializer_list<Vector2> &list) {
		for (Vector2& vec : list) {
			Vector2 thisVal = *this;
			vec = static_cast<Vector2>(static_cast<Vector2>(vec) * thisVal);
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
	friend class Physics;
public:
	//last boolean argument of image sizes is whether you're using the per-component size
	//would be more efficient to have "image sizes" and "is global sizes" as one map, but it is more readable & maintainable to separate them into two.
	Entity(FVector2 _position, float _angle, const std::string &basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*> &dirPaths, IntVec2 size, std::unordered_map<const char *, std::variant<FVector2, FVector2*>> _imageSizes, std::unordered_map<const char *, bool> _isGlobalSize) : position(_position), angle(_angle), pastPosition(_position) {
		rect = new SDL_Rect;
		position.IntoRectXY(rect);
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
				//remember std::get<IntVec2 *> returns type "const IntVec2 *", not "IntVec2 *", thus we must cast before indexing
				static_cast<IntVec2*>(std::get<IntVec2*>(curThisImgSize))[j] = static_cast<FVector2>(static_cast<FVector2*>(std::get<FVector2*>(curImgSize))[j]) * FVecSize;
				j++;
			}
			i++;
		}
	}
	void Finalize();
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
protected:
	FVector2 position;
	FVector2 pastPosition;
	SDL_Rect* rect;
	//per-direction.
	std::variant<IntVec2, IntVec2*>* imageSizes = nullptr;
	bool* isGlobalSize;
};
typedef struct AABB {
public:
	constexpr AABB(FVector2 _min, FVector2 _max): minimum(_min), maximum(_max) {}
	AABB(): minimum(FVector2::Zero), maximum(FVector2::Zero) {}
	Vector2<float> minimum, maximum;
};
struct RigidBody : public Entity {
public:
	//vertices of collider are at entity pos at origin.
	RigidBody(FVector2 _position, FVector2 _velocity, float _angle, const std::string& basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*>& endPaths, IntVec2 size, float _mass, std::initializer_list<FVector2> _narrowPhaseVertices, std::unordered_map<const char*, std::variant<FVector2, FVector2*>> imageSizes = std::unordered_map<const char *, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool> isGlobalSize = std::unordered_map<const char*, bool>(), std::initializer_list<FVector2> _centreOfRotation = std::initializer_list<FVector2>(), FVector2 _centreOfRotForNarrowPVert = FVector2::Zero, IntVec2 _renderOffset = IntVec2::Zero) : mass(_mass), invMass(1.f / _mass), velocity(_velocity), rotation(.0), numNarrowPhaseVertices(_narrowPhaseVertices.size()), origNarrowPVertices(new FVector2[numNarrowPhaseVertices]), flip(SDL_FLIP_NONE),
#ifdef DEBUG_BUILD
		isDebugSquare(false), 
#endif
		renderOffset(_renderOffset), centreOfNarrowPVertRot(_centreOfRotForNarrowPVert), madeAABBTrue(false), isColliding(false), Entity(_position, _angle, basePath, animPaths, endPaths, size, imageSizes, isGlobalSize) {
		SetInitCOR();
		centreOfRotation = new SDL_Point;
		if (_centreOfRotation.size()) {
			memset(centreOfRotation, 0, sizeof(SDL_Point));
		}
		else {
			centreOfRotation->x = size.x / 2;
			centreOfRotation->y = size.y / 2;
		}
		int i = 0;
		for (auto& vec : _narrowPhaseVertices) {
			origNarrowPVertices[i++] = vec;
		}
#ifdef DEBUG_BUILD
		constexpr int numVertInBox = 4;
		if (numNarrowPhaseVertices < numVertInBox) {
			ThrowError((string("rigidbody was submitted which has less than ") + Main::IntToStr(numVertInBox) + " vertices on line " + Main::IntToStr(__LINE__) + " in file " + __FILE__ + ". size is " + Main::IntToStr(numNarrowPhaseVertices) + '.').c_str());
		}
#endif
	}
	int entityIndex;
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
	FVector2 difPositionSection;
	std::atomic<bool> isColliding;
	std::condition_variable collidingCond;
	std::mutex checkColliding;
	const uint numNarrowPhaseVertices;
	FVector2* origNarrowPVertices;
	void SetInitCOR();
	float coefRestitution;
	SDL_Point *centreOfRotation;
	//centre of rotation for the narrow-phase vertices.
	FVector2 centreOfNarrowPVertRot;
	const SDL_RendererFlip flip;
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
	IntVec2 renderOffset;
	Node<IntVec2>* cellHead;
	friend class Physics;
public:
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
	inline void SetSize(IntVec2 size) {
		madeAABBTrue &= (FVector2(rect->w, rect->h) == size);
		size.IntoRectWH(rect);
	}
	inline IntVec2 GetSize() const {
		return { rect->w, rect->h };
	}
	inline bool GetMadeAABBTrue() {
		return madeAABBTrue;
	}
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
	static rbListList* curSortedEntHeadList;
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
public:
	static inline rbList* GetEntHead() {
		return entityHead;
	}
	static Node<RigidBody*>* SubscribeEntity(const std::string &basePath, const std::initializer_list<const char*> &animPaths, const std::initializer_list<const char*> &texturePath, std::initializer_list<FVector2> narrowPhaseVertices = Physics::DefaultSquareVerticesAsList, FVector2 startPos = FVector2::Zero, IntVec2 size = IntVec2::One, std::initializer_list<FVector2> _centreOfRot = std::initializer_list<FVector2>(), FVector2 _centreOfRotNPVert = FVector2::Zero, IntVec2 _renderOffset = IntVec2::Zero, std::unordered_map<const char*, std::variant<FVector2, FVector2 *>> imageSizes = std::unordered_map<const char *, std::variant<FVector2, FVector2*>>(), std::unordered_map<const char*, bool> isGlobalSize = std::unordered_map<const char *, bool>(), FVector2 initVel = FVector2::Zero, float angle = .0f, float mass = 1.f);
	static Node<RigidBody*> *SubscribeEntity(RigidBody *);
	static void Finalize();
	static void Update(float dt);
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
	struct DoubleRB {
		DoubleRB(RigidBody *_a, RigidBody *_b): a(_a), b(_b) {}
		RigidBody* a, * b;
		bool operator == (const DoubleRB & other) const {
			return a == other.a && b == other.b || a == other.b && b == other.a;
		}
	};
	static const std::initializer_list<FVector2> DefaultSquareVerticesAsList;
	static constexpr int numVerticesInSquare = 4;
	static constexpr FVector2 DefaultSquareVertices[numVerticesInSquare] = {
		DEFAULT_SQUARE_POINTS,
	};
	static constexpr int max_quadtree_depth = 10;
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
	QuadNode* topLeft, *topRight, *bottomLeft, *bottomRight;
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
	static constexpr int numNodes = numOfNodes;
	inline QuadNode(FVector2 AABBMin, FVector2 AABBMax): topLeft(nullptr), topRight(nullptr), bottomLeft(nullptr), bottomRight(nullptr), aabb(AABBMin, AABBMax), values(nullptr) {
		_nodes = new std::vector<QuadNode**>({ &topLeft, &topRight, &bottomLeft, &bottomRight });
	}
	inline QuadNode(const AABB &_aabb): topLeft(nullptr), topRight(nullptr), bottomLeft(nullptr), bottomRight(nullptr), aabb(_aabb), values(nullptr) {
		_nodes = new std::vector<QuadNode**>({ &topLeft, &topRight, &bottomLeft, &bottomRight });
	}
	inline AABB GetAABB() {
		return aabb;
	}
	Node<T> *values;
private:
	std::vector<QuadNode**> *_nodes;
public:
	inline void CleanUpNodes() {
		delete _nodes;
	}
	//top-left, top-right, bottom-left, bottom-right
	inline std::vector<QuadNode **> GetNodes() {
		return *_nodes;
	}
	inline bool IsLeafNode() {
		return topLeft == nullptr;
	};
	inline void Enact(std::function<void(Node<T> *, QuadNode<T> *)> del, TypeOfNode typeOfNode) {
		QuadNode* node = nullptr;
		Node<RigidBody*> curNode;
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
			for (uint i = 0; i < numNodes; i++) {
				curNode = GetNodes()[i]->values;
				while (curNode) {
					del(curNode, GetNodes[i]);
					Node<T>::Advance(&curNode);
				}
			}
			return;
			//this redundant label does not hurt performance, since the other labels in the switch statement actually do something, thus the switch statement is not useless, and this label will never be jumped to so it won't detriment performance.
		default:
			ThrowError("case not defined");
			break;
		}
		curNode = node->values;
		while (curNode) {
			del(curNode);
			Node<T>::Advance(&curNode);
		}
	}
	inline void CreateChildNodes() {
		AABB curAABB = AABB();
		bool isLeftNode, isBottomNode;
		bool isBottomLeft;
		FVector2 halfAABB = (aabb.minimum + aabb.maximum) * .5f;
		FVector2 &minAABB = aabb.minimum, &maxAABB = aabb.maximum;
		for (uint i = 0; i < numNodes; i++) {
			/*
			top-left, top-right, bottom-left, bottom-right
			*/
			//there is a much more character-efficient way of doing the nex few lines of code, i could just replace it with a modulo, but in a project filled with those sorts of solutions, it becomes very very difficult to debug.
			isBottomLeft = i == TypeOfNode::tBottomLeft;
			isLeftNode = isBottomLeft || i == TypeOfNode::tTopLeft;
			isBottomNode = isBottomLeft || i == TypeOfNode::tBottomRight;
			curAABB.minimum.x = isLeftNode ? minAABB.x : halfAABB.x;
			curAABB.minimum.y = isBottomNode ? minAABB.y : halfAABB.y;
			curAABB.maximum.x = isLeftNode ? halfAABB.x : maxAABB.x;
			curAABB.maximum.y = isBottomNode ? halfAABB.y : maxAABB.y;
			**(_nodes->begin() + i) = new QuadNode(curAABB);
		}
	}
};