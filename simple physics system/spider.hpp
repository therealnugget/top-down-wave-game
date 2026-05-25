#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class Spider final: public Enemy {
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
	static constexpr float attack_dist_sqr = 8000.f;
	static constexpr float avoid_dist_sqr = 22000.f;
	static constexpr float avoid_spd_multiplier = 3.f;
	static constexpr float max_avoid_time = 4.f;
	static float initSpeed;
	bool lastFramePlrAvoid;
	Timer avoidanceTimer;
public:
	Spider(void);
	~Spider(void) override {
	}
};