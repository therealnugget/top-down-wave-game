#pragma once
#include "physics.hpp"
#include "multicast delegates.hpp"
#include "timer.hpp"
class WhirlPool;
//technically a static class but i'm using a constructor. semantically though, this class can only be constructed once
class WhirlPoolEquipped final: Behaviour {
private:
	void Update(void) override;
	void LateUpdate(void);
	Node<std::function<void(void)>>* lateUpdateNode;
	static constexpr IntVec2 effectSize = IntVec2(170, 170);
	static constexpr float effectSizeDilation = 50.f;
	static constexpr float effectDilationSpeed = 10.f;
	static constexpr float pull_force = 50000.f;
	static constexpr float pull_dist_sqrd = 19000.f;
	static constexpr float initDamage = .3f;
	static float damage;
	static constexpr float no_pull_time = 3.f;
	static constexpr float pull_time = 1.f;
	//needs to be static so the enemies can check for the condition
	static bool isPulling;
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
	static inline float GetDamage(void) {
		return damage;
	}
	static inline bool GetPulling(void) {
		return isPulling;
	}
#ifdef DEBUG_BUILD
	static inline bool IsInstantiated(void) {
		return instantiated;
	}
#endif
	WhirlPoolEquipped(SubRBData *);
	~WhirlPoolEquipped(void) override;
	friend class WhirlPool;
};