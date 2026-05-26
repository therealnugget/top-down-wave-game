#pragma once
#include "physics.hpp"
#include "multicast delegates.hpp"
//technically a static class but i'm using a constructor. semantically though, this class can only be constructed once
class WhirlPoolEquipped final {
private:
	void Update(void);
	Node<std::function<void(void)>>* updateNode;
	static constexpr IntVec2 effectSize = IntVec2(170, 170);
	static constexpr float effectSizeDilation = 50.f;
	static constexpr float effectDilationSpeed = 10.f;
	static constexpr float pull_force = 1000000.f;
	static constexpr float damage_dist_sqrd = 7000.f;
	static constexpr float damage = .3f;
	float aliveSeconds;
	rbList* node;
	RigidBody* rb;
#ifdef DEBUG_BUILD
	static bool instantiated;
#endif
public:
	static inline float GetPullForce() {
		return pull_force;
	}
	static inline float GetDamageDstSqr() {
		return damage_dist_sqrd;
	}
	static inline float GetDamage() {
		return damage;
	}
	WhirlPoolEquipped(void);
	~WhirlPoolEquipped(void);
};