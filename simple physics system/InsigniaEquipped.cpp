#include "main.hpp"
#include "InsigniaEquipped.hpp"
#include "Item.hpp"
#include "player.hpp"
#include "enemy.hpp"
void InsigniaEquipped::LateUpdate(void) {
	rb->SetPosition(Player::GetPosition());
}
void InsigniaEquipped::Update(void) {
	if (!enabled) return;
	if (pullTimer.GetElapsedSeconds() > (effectTime * bEffectActive + noEffectTime * !bEffectActive)) {
		pullTimer.Reset();
		switch (rb->tag) {
		case Main::Tag::whirlPool:
			bWhirlPoolActive = bEffectActive;
			break;
		case Main::Tag::wrath:
			Player::TakeDamage(damages[Main::Tag::wrath] * wrath_plr_dmg_mult);
			break;
		}
		bEffectActive = !bEffectActive;
		damages[rb->tag] = bEffectActive * initDamage;
	}
	rb->SetSize((initEffectSize + IntVec2(FVector2::GetOne() * (sinf(aliveSeconds * effectDilationSpeed) + 1.f) * effectSizeDilation)) * bEffectActive, true);
	aliveSeconds += Main::DeltaTime();
	Behaviour::Update();
}
std::unordered_map<int, float> InsigniaEquipped::damages;
bool InsigniaEquipped::bWhirlPoolActive = false;
InsigniaEquipped::InsigniaEquipped(SubRBData* data, float _noEffectTime, float _effectTime, float _damageAmount, float _effectSizeDilation, float _effectDilationSpeed, float _effectDstSqrd) : bEffectActive(false), aliveSeconds(.0f), pullTimer(Timer(Main::GetRandFloat(.0f, noEffectTime), true)), Behaviour(data) {
	Enemy::AddInsigniaTag(rb->tag);
	damages[rb->tag] = .0f;
	effectSizeDilation = _effectSizeDilation;
	effectDilationSpeed = _effectDilationSpeed;
	effectDistSqrd = _effectDstSqrd;
	initEffectSize = entity->GetSize();
	effectTime = _effectTime;
	noEffectTime = _noEffectTime;
	initDamage = _damageAmount;
	SetUpdateNode(Main::Updates += [this]()->void {Update(); });
	lateUpdateNode = Main::LateUpdates += [this]()->void {LateUpdate(); };
}
InsigniaEquipped::~InsigniaEquipped(void) {
	if (lateUpdateNode) Main::LateUpdates -= lateUpdateNode;
}