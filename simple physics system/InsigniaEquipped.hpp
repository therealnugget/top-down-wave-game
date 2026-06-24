#pragma once
#include "physics.hpp"
#include "multicast delegates.hpp"
#include "timer.hpp"
class Insignia;
class InsigniaEquipped final: Behaviour {
private:
	void Update(void) override;
	void SetEffectActive(int);
	void LateUpdate(void);
	Node<std::function<void(void)>>* lateUpdateNode;
	float effectSizeDilation;
	float effectDilationSpeed;
	static constexpr float pull_force = 900000.f;
	static constexpr float wrath_plr_dmg_mult = .1f;
	static bool bWhirlPoolActive;
	float effectDistSqrd;
	float initDamage;
	IntVec2 initEffectSize;
	//needs to be static so the enemies can check for the condition
	static std::unordered_map<int, float> damages;
	float noEffectTime;
	float effectTime;
	bool bEffectActive;
	Timer pullTimer;
	float aliveSeconds;
public:
	static inline float GetPullForce(void) {
		return pull_force;
	}
	inline float GetEffectDstSqr(void) {
		return effectDistSqrd;
	}
	inline static float GetDamage(int tag) {
		return damages[tag];
	}
	inline static bool GetWhirlPoolActive(void) {
		return bWhirlPoolActive;
	}
	InsigniaEquipped(SubRBData *, float noEffectTime, float effectTime, float damage, float effectSizeDilation, float effectDilationSpd, float effectDistSqrd);
	~InsigniaEquipped(void) override;
	friend class Insignia;
};