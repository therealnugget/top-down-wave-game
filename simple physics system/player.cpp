#include "player.hpp"
#include "textures.hpp"
#include "winMgr.hpp"
#include <tuple>
#include "create shapes.hpp"
#include "array.hpp"
#include "main.hpp"
#include <algorithm>
#include "animations.hpp"
#include "multicast delegates.hpp"
#include "Item.hpp"
//#define IS_DEV
#define PLAYER_WIDTH 70
#define PLAYER_HEIGHT 70
float Player::accel = 700000.f;
float Player::speed = 2000.f;
float Player::knockBack = 600.f;
float Player::plrAttkET = .0f;
float Player::maxHealth = 55.f;
float Player::health = Player::maxHealth;
float Player::crystalColldierSizeMult = 1.4f;
FVector2 Player::mouseDiff;
static constexpr float attackSizeMult = 1.2f;
const FVector2 Player::playerCollider = FVector2(.375f, .5f);
IntVec2 Player::attackSize = IntVec2(PLAYER_WIDTH * attackSizeMult, PLAYER_HEIGHT * attackSizeMult);
IntVec2 Player::pastInp;
bool Player::mouseVertical;
bool Player::colOnFrame = false;
rbList* Player::plrNode;
rbList* Player::plrAttack = nullptr;
RigidBody *Player::player;
Behaviour *Player::plrBehaviour;
Entity* Player::playerEnt;
static FVector2 currentVel;
//#define SHOW_AABB
#ifdef SHOW_AABB
static void SetPositions(rbList** rbs, int i, int j, RigidBody *rb) {
	rbs[i]->value->SetPosition(rb->GetNarrowPhaseVertices()[j] + ((static_cast<FVector2>(FVector2::Left) + FVector2::Down) * 2.5f));
}
#endif
static IntVec2 playerSize = IntVec2(static_cast<float>(PLAYER_WIDTH), static_cast<float>(PLAYER_HEIGHT));
static IntVec2 playerSizeFVec = static_cast<FVector2>(playerSize);
FVector2 Player::healthBarOffset = { -21.f, -35.f };
IntVec2 Player::healthBarSize = IntVec2(40, 20);
FVector2 Player::progressBarPos = { .0, .0f };
IntVec2 Player::progressBarInitSize = IntVec2(2000, 100);
Node<Entity*> *Player::healthbar;
Node<RigidBody*>* Player::crystalCollider;
RigidBody* Player::crystalColliderRb;
Entity* Player::healthBarEnt;
void Player::TakeDamage(void) {
#ifdef IS_DEV
	return;
#endif
	if (PlayingHurtAnim()) return;
	health--;
	PlayDirAnim(hit, pastInp);
	if (health <= .0f) {
		Main::SetPauseState(true);
	}
}
void Player::Init(void) {
#define USE_NORMAL_PLAYER_POS
	FVector2 defaultPlrPos = 
		#ifdef USE_NORMAL_PLAYER_POS
		Main::halfDisplaySize + (static_cast<FVector2>(FVector2::Down) + FVector2::Left) * playerSizeFVec * .5f
		#else
		FVector2::Zero
#endif
		;
	auto crystalData = SubRBData("", std::vector<const char*>(), playerSizeFVec * crystalColldierSizeMult * Physics::DefaultSquareVerticesVec, defaultPlrPos);
	crystalData.isTrigger = true;
	crystalData.tag = Main::Tag::playerTrigCrystal;
	crystalData.createEntity = false;
	crystalCollider = Physics::SubscribeEntity(&crystalData);
	crystalColliderRb = crystalCollider->value;
	plrBehaviour = new Behaviour(SubRBData("main/Char_Sprites", Animations::MakeAnimStrs(numAnims, idle, "idle", run, "run", attack, "attack", hit, "hit"), static_cast<FVector2>(playerCollider) * Physics::DefaultSquareVerticesVec, defaultPlrPos, playerSize, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f, Main::Tag::player));
	plrNode = plrBehaviour->rbNode;
	player = plrNode->value;
	player->SetTrigger(true);
	player->updateNode = Main::Updates += Player::Update;
	Main::LateUpdates += Player::LateUpdate;
	player->SetCollisionCallback([](Collision *collision) -> void {
		if (!collision->CompareTag(Main::Tag::enemy) || colOnFrame) return;
		colOnFrame = true;
		TakeDamage();
		});
	playerEnt = player->GetEntity();
	PlayDirAnim(idle);
	playerEnt->SetNotLoopDirs(Main::GetAnimOffset(hit));
	playerEnt->SetRecordAnim(true);
	healthbar = Physics::SubStandaloneEnt(Entity::MakeEntity("health bar", { "health_bar" }, player->GetPosition() + healthBarOffset, healthBarSize));
	progressBarPos.x = Main::halfDisplaySize.x * .0f;
	auto progressBar = Physics::SubStandaloneEnt(Entity::MakeEntity(Main::empty_string, { "progress_bar" }, progressBarPos, progressBarInitSize));
	healthBarEnt = healthbar->value;
	healthBarEnt->SetNotLoop(healthBarAnim);
	constexpr int numShapes = 0;
	if (!numShapes) return;
	//player2 = Shapes::CreateShape(Physics::DefaultSquareVerticesAsList, defaultPlrPos, playerSize, 1.f, Shapes::square, std::initializer_list<FVector2>(), FVector2::Zero, -playerSize * .5f);
	constexpr float scaleFact = .1f;
	FVector2 shapeSize = static_cast<FVector2>(playerSize) * scaleFact;
	constexpr float border = .05f;
	constexpr float invBorder = 1.f - border;
	//don't use createshapes using first param as numshapes here, because we need to randomize the positions.
	for (int i = 0; i < numShapes; i++) Shapes::CreateShapes(1, /*Main::halfDisplaySize + FVector2::GetRight() * (100.f + (static_cast<float>(static_cast<bool>(i & 1)) * 2.f - 1.f) * static_cast<float>(i) * .5f * shapeSize.x)*/Main::GetRandFVec(static_cast<const FVector2>(static_cast<FVector2>(Main::DisplaySize) * border), Main::DisplaySize * invBorder), shapeSize, scaleFact, Shapes::blueSqr, std::initializer_list<FVector2>(), FVector2::Zero, -shapeSize * .5f, false);//nb: this line isn't causing the current error
}
static constexpr float playerAttackSizeMult = 1.2f;
static constexpr float playerAttackOut = .2f;
void Player::LateUpdate(void) {
	healthBarEnt->SetRectPosition(GetPosition() + healthBarOffset);
	crystalColliderRb->SetPosition(GetPosition());
}
void Player::Update(void) {
	colOnFrame = false;
	if (Main::KeyPressed(SDL_SCANCODE_K)) {
		for (int i = 0; i < 3; i++) {
			constexpr IntVec2 itemTextSize = IntVec2(150, 100);
			constexpr int itemYSeparation = 190;
			new Text(IntVec2::GetOne() * itemTextSize, Main::halfDisplaySize + IntVec2::GetUp() * (i - 1) * itemYSeparation, "test", true);
		}
		Main::TogglePauseState();
		Main::canChangePause = false;
	}
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
	healthBarEnt->SetAnimFrame(static_cast<int>(floorf(GetHealthFrac() * static_cast<float>(healthBarEnt->GetNumAnimFrames() - 1))));
	if (Main::GetKey(SDL_SCANCODE_O)) player->SetRotation(player->GetRotation() + rotationSpd * Main::DeltaTime());
	if (!Main::leftClick && plrAttack) {
		Physics::DeleteRB(plrAttack);
		plrAttack = nullptr;
	}
	if (PlayingHurtAnim()) return;
	if (Main::leftClick) {
		auto pos = player->GetPosition();
		mouseDiff = (static_cast<FVector2>(Main::mousePosition) - pos).Normalized();
		auto mouseRightDot = GetPlayerRightNorm() ^ mouseDiff;
		auto mouseUp = (GetPlayerUpNorm() ^ mouseDiff) >= .0f;
		mouseVertical = mouseRightDot < .5f && mouseRightDot > -.5f;
		auto mouseDirection = IntVec2(!mouseVertical * ((mouseRightDot >= .0f) * 2 - 1), mouseVertical * (mouseUp * 2 - 1));
		PlayDirAnim(attack, mouseDirection);
		pastInp = mouseDirection;
		auto plrAttkPos = pos + static_cast<FVector2>(mouseDiff * playerSize.x * playerAttackOut);
		if (!plrAttack) {
			auto data = SubRBData("sword slash"s, { "sword_slash" }, Physics::DefaultSquareVerticesVec, plrAttkPos, attackSize, std::initializer_list<FVector2>(), FVector2::Zero, attackSize * -.5f, Main::Tag::playerAttack, nullptr, std::unordered_map<std::string, std::variant<FVector2, FVector2*>>(), std::unordered_map<std::string, bool>(), FVector2::Zero, mouseDiff.Angle(), 1.f, true, true, { Main::empty_cc });
			plrAttack = Physics::SubscribeEntity(&data);
			auto attackRB = plrAttack->value;
			auto attackEnt = attackRB->GetEntity();
			attackEnt->SetNotLoop(attkSlashAnim);
			attackRB->SetFricCoef(.0f);
			attackEnt->SetAnimSpd(attackSlashAnimSpeed);
			plrAttkET = .0f;
		}
		auto attackEnt = plrAttack->value->GetEntity();
		if (attackEnt->AnimFinished()) {
			attackEnt->ResetAnim(attkSlashAnim);
			plrAttkET = .0f;
		}
		plrAttack->value->SetRotation(static_cast<double>(mouseDiff.Angle()) + 180.f);
		plrAttkET += Main::DeltaTime();
		plrAttack->value->SetPosition(plrAttkPos + mouseDiff * plrAttkET * plrAttkOutSpd);
		return;
	}
	if (Main::moving) {
		Player::PlayDirAnim(run, Main::iInputVec);
		pastInp = Main::iInputVec;
		return;
	}
	Player::PlayDirAnim(idle, pastInp);
}
void Player::PlayDirAnim(int animation, IntVec2 direction) {
	playerEnt->SetAnimation(IntVec2::VecToDir(direction) + Main::GetAnimOffset(animation));
}