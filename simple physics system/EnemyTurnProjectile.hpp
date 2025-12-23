#pragma once
#include "timer.hpp"
#include "multicast delegates.hpp"
#include "physics.hpp"
#include "math.hpp"
class EnemyTurnProjectile final {
private:
	static constexpr int projectile_frame_wait = 64;
	static constexpr int num_projectiles = 3;
	static constexpr IntVec2 projectileSize = IntVec2(40, 40);
	static constexpr float randRotationOffset = 20.f * Math::deg2rad;
	static constexpr float baseSpeed = .5f;
	FVector2 mouseDiff;
	FVector2 GetProjectilePos(int);
	int currentProjectileFrame;
	bool makeNewSeeds[num_projectiles];
	float randRots[num_projectiles];
	Timer projectileTime;
	rbList* projectileNodes[num_projectiles];
	RigidBody* projectileRBs[num_projectiles];
	void Update(void);
	void DestroyProjectile(void);
public:
	EnemyTurnProjectile(void) : projectileNodes({}), projectileRBs({}), makeNewSeeds({}), currentProjectileFrame(0) {
#ifdef DEBUG_BUILD
		Assert(Main::IsPowTwo(projectile_frame_wait), "projectile_frame_wait should be a power of two");
#endif
		Main::Updates += [this]() {Update(); };
		projectileTime.Reset();
	}
};