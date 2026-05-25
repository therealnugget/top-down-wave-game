#pragma once
#include "enemy.hpp"
#include "player.hpp"
#include "multicast delegates.hpp"
class Ghost final : public Enemy {
private:
	void TakeDamage(float damageAmount) override;
	void Update(void) override;
	enum GhostAnimation {
		idle,
		death,
		hurt,
		run,
		numGhostAnims,
	};
	static constexpr float attackDistSqr = 8000.f;
	static constexpr float avoidanceDistSqr = 24000.f;
	static constexpr float max_avoid_time_teleport = 7.f;
	static constexpr float speed_mul_teleport_time = 1.f;
	static constexpr float speed_teleport_factor = 1.4f;
	static constexpr float teleport_out_factor = 80.f;
	static constexpr int random_spawn_one_in = 3;
	static float initSpeed;
	bool temp;
	Timer avoidanceTimer;
	bool lastFramePlrClose;
public:
	Ghost(void);
	~Ghost(void) override {
	}
};