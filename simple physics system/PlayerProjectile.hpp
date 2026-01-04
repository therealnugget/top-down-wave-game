#pragma once
#include "timer.hpp"
#include "multicast delegates.hpp"
#include "physics.hpp"
#include "math.hpp"
class PlayerProjectile final {
private:
	int projectileFrameWait;
	int numProjectiles;
	IntVec2 projectileSize;
	float randRotationOffset;
	float baseSpeed;
	FVector2 mouseDiff;
	FVector2 GetProjectilePos(int);
	int currentProjectileFrame;
	std::vector<bool>makeNewSeeds;
	std::vector<float> randRots;
	std::vector<rbList *>projectileNodes;
	std::vector<RigidBody *>projectileRBs;
	std::string basePath;
	const char* endPath;
	Timer projectileTime;
	void Update(void);
	void DestroyProjectile(void);
	int tag;
public:
	PlayerProjectile(std::string _basePath, const char* _endPath, int _numProjectiles = 6, int _tag = Main::Tag::enemyTurner, int _projectileFrameWait = 32, IntVec2 _projectileSize = IntVec2(20, 20), float _randRotationOffset = 25.f * Math::deg2rad, float _baseSpeed = .5f) : projectileFrameWait(_projectileFrameWait), projectileSize(_projectileSize), randRotationOffset(_randRotationOffset), baseSpeed(_baseSpeed), numProjectiles(_numProjectiles), makeNewSeeds(std::vector<bool>(numProjectiles)), randRots(std::vector<float>(numProjectiles)), projectileNodes(std::vector<rbList*>(numProjectiles)), projectileRBs(std::vector<RigidBody*>(numProjectiles)), basePath(_basePath), endPath(_endPath), currentProjectileFrame(0) {
		tag = _tag;
#ifdef DEBUG_BUILD
		Assert(Main::IsPowTwo(projectileFrameWait), "projectile_frame_wait should be a power of two");
#endif
		Main::Updates += [this]() {Update(); };
		projectileTime.Reset();
	}
};