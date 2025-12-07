#pragma once
#include "debug.hpp"
#include "physics.hpp"
#include <SDL.h>
//static, therefore can't inherit from behaviour.
class Player final {
private:
	static constexpr float rotationSpd = 500.f;
	static constexpr float attackSlashAnimSpeed = 1.2f;
	static constexpr float plrAttkOutSpd = 600.f;
	static constexpr int attkSlashAnim = 0;
	static constexpr int healthBarAnim = 0;
	static float plrAttkET;
	static float maxHealth;
	static float health;
	static Node<std::function<void(void)>>* updateNode;
	static Behaviour* plrBehaviour;
	static rbList* plrNode;
	static rbList* plrAttack;
	static RigidBody* player;
	static Entity* playerEnt;
	static Node<Entity*> *healthbar;
	static Entity* healthBarEnt;
	static float accel, speed;
	static float knockBack;
	static FVector2 mouseDiff;
	static FVector2 healthBarOffset;
	static IntVec2 healthBarSize;
	static IntVec2 attackSize;
	static IntVec2 pastInp;
	static bool mouseVertical;
	static bool colOnFrame;
	static void PlayDirAnim(int animation = -1, IntVec2 = IntVec2::Zero);
public:
	static inline float GetHealthFrac() {
		return health / maxHealth;
	}
	//normal of mouse pos from plr pos
	static inline FVector2 GetMouseDiff() {
		return mouseDiff;
	}
	static inline float GetKnockBack() {
		return knockBack;
	}
	static inline FVector2 GetPlayerRightNorm() {
		return FVector2(cosf(player->GetRotation()), sinf(player->GetRotation()));
	}
	static inline FVector2 GetPlayerUpNorm() {
		auto upRotation = player->GetRotation() + 90.f;
		return FVector2(cosf(upRotation), sinf(upRotation));
	}
	static inline FVector2 GetPosition() {
		return player->GetPosition();
	}
	static void TakeDamage(void);
	static void Init(void);
	static void Finalize(void);
	static void Update(void);
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