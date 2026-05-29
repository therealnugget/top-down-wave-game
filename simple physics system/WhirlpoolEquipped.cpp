#include "main.hpp"
#include "WhirlpoolEquipped.hpp"
#include "Item.hpp"
#include "player.hpp"
void WhirlPoolEquipped::LateUpdate(void) {
	rb->SetPosition(Player::GetPosition());
}
void WhirlPoolEquipped::Update(void) {
	if (!enabled) return;
	if (pullTimer.GetElapsedSeconds() > (pull_time * isPulling + no_pull_time * !isPulling)) {
		pullTimer.Reset();
		isPulling = !isPulling;
		damage = isPulling * initDamage;
	}
	rb->SetSize((effectSize + IntVec2(FVector2::GetOne() * (sinf(aliveSeconds * effectDilationSpeed) + 1.f) * effectSizeDilation)) * isPulling, true);
	aliveSeconds += Main::DeltaTime();
	Behaviour::Update();
}
#ifdef DEBUG_BUILD
bool WhirlPoolEquipped::instantiated = false;
#endif
bool WhirlPoolEquipped::isPulling = false;
float WhirlPoolEquipped::damage = .0f;
WhirlPoolEquipped::WhirlPoolEquipped(SubRBData *data) : aliveSeconds(.0f), pullTimer(Timer(no_pull_time + 1.f, true)), Behaviour(data) {
#ifdef DEBUG_BUILD
	if (instantiated) {
		ThrowError("there should not be more than one instance of the \"WhirlPoolEquipped\" class.");
	}
	instantiated = true;
#endif
	SetUpdateNode(Main::Updates += [this]()->void {Update(); });
	lateUpdateNode = Main::LateUpdates += [this]()->void {LateUpdate(); };
}
WhirlPoolEquipped::~WhirlPoolEquipped(void) {
	if (lateUpdateNode) Main::LateUpdates -= lateUpdateNode;
}