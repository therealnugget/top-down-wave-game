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
	static constexpr float pull_force = 50000.f;
	static constexpr float pull_dist_sqrd = 19000.f;
	static bool bWhirlPoolActive;
	float effectDistSqrd;
	float initDamage;
	IntVec2 initEffectSize;
	//needs to be static so the enemies can check for the condition
	static std::vector<float> damages;
	float noEffectTime;
	float effectTime;
	bool bEffectActive;
	Timer pullTimer;
	float aliveSeconds;
#ifdef DEBUG_BUILD
	static bool instantiated;
#endif
public:
	static inline float GetPullForce(void) {
		return pull_force;
	}
	static inline float GetPullDstSqr(void) {
		return pull_dist_sqrd;
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
#ifdef DEBUG_BUILD
	static inline bool IsInstantiated(void) {
		return instantiated;
	}
#endif
	InsigniaEquipped(SubRBData *, float noEffectTime, float effectTime, float damage, float effectSizeDilation, float effectDilationSpd, float effectDistSqrd);
	~InsigniaEquipped(void) override;
	friend class Insignia;
};