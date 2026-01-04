#pragma once
#include "debug.hpp"
#include "physics.hpp"
#include <SDL.h>
//static, therefore can't inherit from behaviour.
class Player final {
private:
	static constexpr float rotationSpd = 500.f;
	static constexpr float attackSlashAnimSpeed = 1.2f;
	static constexpr float hurtAnimSpeed = 2.f;
	static float projectileSpd;
	static constexpr int attkSlashAnim = 0;
	static constexpr int healthBarAnim = 0;
	static const FVector2 playerCollider;
	static float crystalColldierSizeMult;
	static float plrAttkET;
	static float maxHealth;
	static float health;
	static float accel, speed;
	static float knockBack;
	static float damage;
	static float progressAmount;
	static float maxProgress;
	static float progressIncrease;
	static bool mouseVertical;
	static bool colOnFrame;
	static bool enabled;
	static Node<std::function<void(void)>>* updateNode;
	static Behaviour* plrBehaviour;
	static rbList* plrNode;
	static rbList* plrAttack;
	static RigidBody* player;
	static Entity* playerEnt;
	static Node<Entity*> *healthbar;
	static Entity* healthBarEnt;
	static Node<Entity*> *progressBar;
	static Entity* progressBarEnt;
	static Node<RigidBody*> *crystalCollider;
	static RigidBody* crystalColliderRb;
	static FVector2 mouseDiff;
	static FVector2 healthBarOffset;
	static FVector2 progressBarPos;
	static IntVec2 healthBarSize;
	static IntVec2 progressBarInitSize;
	static IntVec2 attackSize;
	static IntVec2 pastInp;
	static IntVec2 plrAttkPos;
	static void PlayDirAnim(int animation = -1, IntVec2 = IntVec2(-1, 0));
public:
	static inline float GetDamage(void) {
		return damage;
	}
	static IntVec2 GetProjectilePos(float, FVector2);
	static rbList *CreatePlayerProjectile(std::string basePath, const char *endPath, IntVec2 position, IntVec2 size, RigidBody ** outRb = nullptr, Entity ** outEnt = nullptr, float rotation = .0f);
	static inline float GetHealthFrac(void) {
		return health / maxHealth;
	}
	//normal of mouse pos from plr pos
	static inline FVector2 GetMouseDiff(void) {
		return (static_cast<FVector2>(Main::GetMousePosition()) - GetPosition()).Normalized();
	}
	static inline float GetKnockBack(void) {
		return knockBack;
	}
	static inline FVector2 GetPlayerRightNorm(void) {
		return FVector2(cosf(player->GetRotation()), sinf(player->GetRotation()));
	}
	static inline FVector2 GetPlayerUpNorm(void) {
		auto upRotation = player->GetRotation() + 90.f;
		return FVector2(cosf(upRotation), sinf(upRotation));
	}
	static inline FVector2 GetPosition(void) {
		return player->GetPosition();
	}
	static void TakeDamage(float);
	static void IncreaseProgress(float);
	static void IncreasePickupRange(float);
	static void IncreaseHealth(float);
	static void ReplenishHealth(void);
	static void Init(void);
	static void Finalize(void);
	static void Update(void);
	static void LateUpdate(void);
	inline static bool PlayingHurtAnim() {
		return playerEnt->GetCurAnim() == Main::GetAnimOffset(hit) + IntVec2::VecToDir(pastInp) && !playerEnt->AnimFinished();
	}
	friend class Main;
	static enum PlrAnim {
		idle,
		run,
		attack,
		hit,
		numAnims,
	};
};