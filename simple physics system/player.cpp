#include "player.hpp"
#include "textures.hpp"
#include "winMgr.hpp"
#include <tuple>
#include "create shapes.hpp"
#include "array.hpp"
#include "linkedList.hpp"
#include <algorithm>
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 50
float Player::accel = 30000.f;
float Player::speed = 1000.f;
static RigidBody *player;
static rbList *plrNode;
static rbList* player2;
static RigidBody* player2Rb;
std::unordered_map<const char*, SDL_Texture*> Images::loadedTexs;
std::unordered_map<const char*, SDL_Surface*> Images::loadedSurfaces;
std::initializer_list<const char *> Player::anims;
std::unordered_map<int, const char*> Player::dirNames = {
	{Main::down, "down"},
	{Main::left, "left"},
	{Main::up, "up"},
	{Main::right, "right"},
};
std::unordered_map<int, const char*> Player::animNames = {
	{Player::idle, "idle"},
	{Player::run, "run"},
	{Player::attack, "attack"},
};
static FVector2 currentVel;
rbList** shapesTemp;
rbList** shapesTemp2;
IntVec2 Player::pastInp = FVector2::Down;
//#define SHOW_AABB
#ifdef SHOW_AABB
static void SetPositions(rbList** rbs, int i, int j, RigidBody *rb) {
	rbs[i]->value->SetPosition(rb->GetNarrowPhaseVertices()[j] + ((static_cast<FVector2>(FVector2::Left) + FVector2::Down) * 2.5f));
}
#endif
void Player::Init() {
	Main::Updates += Player::Update;
	FVector2 playerSize = FVector2(static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT));
	//#define MAKE_SHAPES
	int i;
#ifdef MAKE_SHAPES
	constexpr uint num_shapes_temp = 9000;
	constexpr uint_fast8_t entropyFactor = 10;
	for (int i = 0; i < entropyFactor; i++) rand();
	auto GetRandomVal = []() {
		return static_cast<double>(rand() - RAND_MAX / 2) / static_cast<double>(RAND_MAX / 2);
		};
	Node<RigidBody*>** shapes = Shapes::CreateShapes(num_shapes_temp, Physics::DefaultSquareVerticesAsList, Main::halfDisplaySize, IntVec2::One);
	for (int i = 0; i < num_shapes_temp; i++) {
		shapes[i]->value->SetSize(static_cast<IntVec2>(IntVec2::One) * (GetRandomVal() + 2.0) * 2);
		shapes[i]->value->AddForce((static_cast<FVector2>(FVector2::Right) * GetRandomVal() + static_cast<FVector2>(FVector2::Down) * GetRandomVal()) * 62000.0);
	}
#else
#ifdef SHOW_AABB
	constexpr uint_fast8_t numShapesTemp = 2;

	auto AssignShape = [numShapesTemp](rbList**& rbs, Shapes::TypeOfShape typeOfShape) {
		rbs = Shapes::CreateShapes(numShapesTemp, Physics::DefaultSquareVerticesAsList, FVector2::Zero, static_cast<IntVec2>(IntVec2::One) * 5, 1.f, typeOfShape);
		};
	AssignShape(shapesTemp, Shapes::TypeOfShape::blueSqr);
	AssignShape(shapesTemp2, Shapes::TypeOfShape::greenSqr);
	auto SetDbgSqr = [i](rbList** rbs) {
		rbs[i]->value->isDebugSquare = true;
		};
	for (i = 0; i < numShapesTemp; i++) {
		SetDbgSqr(shapesTemp);
		SetDbgSqr(shapesTemp2);
	}
#endif
#endif
#define USE_NORMAL_PLAYER_POS
	FVector2 defaultPlrPos = 
		#ifdef USE_NORMAL_PLAYER_POS
		Main::halfDisplaySize + (static_cast<FVector2>(FVector2::Down) + FVector2::Left) * playerSize * .5f
		#else
		FVector2::Zero
#endif
		;
	auto endPaths = std::vector<const char*>();
	endPaths.reserve(Main::num_directions);
	for (i = 0; i < Main::num_directions; i++) {
		endPaths.push_back(dirNames[i]);
	}
	const auto endPathsList = Main::VecToInitList<const char*>(endPaths);
	std::vector<const char *> animStrs = std::vector<const char*>();
	animStrs.reserve(numAnims);
	const char* curName;
	for (i = 0; i < numAnims; i++) {
		curName = animNames[i];
		animStrs.push_back(curName);
	}
	anims = Main::VecToInitList<const char*>(animStrs);
	const auto repVec = FVector2::GetRepeatingVec(FVector2(2.f, 3.f), FVector2(3.f, 2.f));
	//std::unordered_map<const char*, std::variant<FVector2, FVector2 *>> imageSizes = { {"run", std::variant<FVector2, FVector2*>(FVector2::One)}, { "idle", std::variant<FVector2, FVector2*>(FVector2::One) }, {"attack", std::variant<FVector2, FVector2*>(repVec)} };
	_freea(repVec);
	std::unordered_map<const char*, int> isGlobalSize = { true, true, false };
	plrNode = Physics::SubscribeEntity("Top_Down_Adventure_Pack_v.1.0/Char_Sprites/", anims, endPathsList, Physics::DefaultSquareVerticesAsList, defaultPlrPos + FVector2::GetRight() * playerSize, playerSize, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f, std::unordered_map<const char *, int>(), isGlobalSize);
	constexpr int numShapes = 0;
	player = plrNode->value;
	if (!numShapes) return;
	//player2 = Shapes::CreateShape(Physics::DefaultSquareVerticesAsList, defaultPlrPos, playerSize, 1.f, Shapes::square, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f);
	constexpr float scaleFact = .1f;
	FVector2 shapeSize = playerSize * scaleFact;
	std::initializer_list<FVector2> shapeColVertices = Physics::DefaultSquareVerticesAsList;
	for (auto& vec : shapeColVertices) {
		static_cast<FVector2>(vec) *= scaleFact;
	}
	constexpr float border = .05f;
	constexpr float invBorder = 1.f - border;
	for (int i = 0; i < numShapes; i++) Shapes::CreateShape(shapeColVertices, Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize) * border), Main::DisplaySize * invBorder), shapeSize, scaleFact, Shapes::blueSqr, std::initializer_list<FVector2>(), FVector2::Zero, -shapeSize * .5f);
	//player2Rb = player2->value;
}
void Player::PlayAnim(int animation) {
	player->SetAnimation(IntVec2::VecToDir(pastInp) + Main::GetAnimOffset(animation));
}
void Player::Update(void) {
	player->AddForce(Main::fInputVec * accel);
	//player2Rb->AddForce(Main::fInputVec2 * accel);
	currentVel = player->GetVelocity();/*
	cout << "player is located at ";
	player->GetPosition().PrintVec();
	cout << "and player 2 is located at ";
	player2Rb->GetPosition().PrintVec();*/
	if (currentVel.Magnitude() > speed) player->SetVelocity(currentVel.Normalized() * speed);
	//currentVel = player2Rb->GetVelocity();
	//if (currentVel.Magnitude() > speed) player2Rb->SetVelocity(currentVel.Normalized() * speed);
#ifdef SHOW_AABB
	for (int i = 0; i < 2; i++) {
		int j = i ? 2 : 0;
		//the last term is just to correct for the size of the square itself.
		SetPositions(shapesTemp, i, j, player);
		SetPositions(shapesTemp2, i, j, player2Rb);
		/*cout << "vertex at " << std::to_string(j) << " is ";
		player->GetNarrowPhaseVertices()[j].PrintVec();*/
	}
#endif
	if (Main::fInputVec == FVector2::Zero) {
		if (Main::GetKey(SDL_SCANCODE_SPACE)) {
			Player::PlayAnim(attack);
			return;
		}
		Player::PlayAnim(idle);
		return;
	}
	Player::PlayAnim(run);
	pastInp = Main::iInputVec;
}