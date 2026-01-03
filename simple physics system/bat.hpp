#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class Bat final: public Enemy {
private:
	void TakeDamage(float damageAmount) override;
	void Update(void) override;
	enum BatAnimation {
		idle,
		death,
		hurt,
		run,
		numBatAnims,
	};
	static constexpr float attackDistSqr = 8000.f;
public:
	Bat(void);
	~Bat(void) override {
	}
};