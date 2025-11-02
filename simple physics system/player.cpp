#include "player.hpp"
#include "textures.hpp"
#include "winMgr.hpp"
#include <tuple>
#include "create shapes.hpp"
#include "array.hpp"
#include "linkedList.hpp"
#include <algorithm>
#include "animations.hpp"
#define PLAYER_WIDTH 150
#define PLAYER_HEIGHT 150
float Player::accel = 700000.f;
float Player::speed = 1500.f;
static RigidBody *player;
static Entity* playerEnt;
static rbList *plrNode;
static FVector2 currentVel;
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
#define USE_NORMAL_PLAYER_POS
	FVector2 defaultPlrPos = 
		#ifdef USE_NORMAL_PLAYER_POS
		Main::halfDisplaySize + (static_cast<FVector2>(FVector2::Down) + FVector2::Left) * playerSize * .5f
		#else
		FVector2::Zero
#endif
		;
	plrNode = Physics::SubscribeEntity("Top_Down_Adventure_Pack_v.1.0/Char_Sprites", Animations::MakeAnimStrs(numAnims, idle, "idle", run, "run", attack, "attack"), FVector2(.375f, .5f) * Physics::DefaultSquareVerticesVec, defaultPlrPos, playerSize, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5, Main::Tag::player);
	player = plrNode->value;
	player->SetCollisionCallback([](auto &collision) -> void {
		//std::cout << "colliding!!!\n";
		});
	playerEnt = player->GetEntity();
	playerEnt->SetRecordAnim(true);
	constexpr int numShapes = 200;
	if (!numShapes) return;
	//player2 = Shapes::CreateShape(Physics::DefaultSquareVerticesAsList, defaultPlrPos, playerSize, 1.f, Shapes::square, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f);
	constexpr float scaleFact = .1f;
	FVector2 shapeSize = playerSize * scaleFact;
	constexpr float border = .05f;
	constexpr float invBorder = 1.f - border;
	//don't use createshapes using first param as numshapes here, because we need to randomize the positions.
	for (int i = 0; i < numShapes; i++) Shapes::CreateShapes(1, /*Main::halfDisplaySize + FVector2::GetRight() * (100.f + (static_cast<float>(static_cast<bool>(i & 1)) * 2.f - 1.f) * static_cast<float>(i) * .5f * shapeSize.x)*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize) * border), Main::DisplaySize * invBorder), shapeSize, scaleFact, Shapes::blueSqr, std::initializer_list<FVector2>(), FVector2::Zero, -shapeSize * .5f, false);//nb: this line isn't causing the current error
}
void Player::PlayAnim(int animation) {
	playerEnt->SetAnimation(IntVec2::VecToDir(pastInp) + Main::GetAnimOffset(animation));
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
	/*
	if (temp) Physics::DeleteRB(temp);
	temp = Physics::StandaloneRB(FVector2(static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT)), Main::halfDisplaySize + FVector2::GetRight() * 300.f);*/
	if (Main::GetKey(SDL_SCANCODE_O)) player->SetRotation(player->GetRotation() + rotationSpd * Main::DeltaTime());
	if (Main::GetKey(SDL_SCANCODE_SPACE)) {
		Player::PlayAnim(attack);
		Player::SetPastInp();
		return;
	}
	Player::SetPastInp([]() -> void {Player::PlayAnim(run); }, []() -> void {Player::PlayAnim(idle); });
}