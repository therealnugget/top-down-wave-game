#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class SwordGuy final: public Enemy {
private:
	void CheckResetAttackDecision(void);
	void SetPlayerDist(void) override;
	void EnactDamage(void) override;
	void TakeDamage(float damageAmount) override;
	void Update(void) override;
	enum SGAnimation {
		idle,
		death,
		hurt,
		walk,
		run,
		jump,
		overhead,
		swing,
		dash,
		numSGAnims,
	};
	static constexpr float dashPositionAdd = 200.f;
	static constexpr float attackDistSqr = 10000.f;
	static constexpr float dashDistSqr = 17000.f;
	//dash + 1 for yet to decide
	int attackDecision;
	bool attackDecide;
	bool animFinished;
	float signAtDash;
	void ResetIfAttkFin(int animation);
public:
	SwordGuy(void);
	~SwordGuy(void) override {
	}
};