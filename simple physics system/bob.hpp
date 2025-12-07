#pragma once
#include "enemy.hpp"
#include "math.hpp"
#include "player.hpp"
class SwordGuy final: public Enemy {
private:
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
	//-1 for yet to decide
	int attackDecision;
	bool decideAttack;
	float speed;
	float plrDistSqr;
	float signAtDash;
	bool animFinished;
	FVector2 toPlr;
	static constexpr float attackDistSqr = 10000.f;
	static constexpr float dashDistSqr = 17000.f;
	void CollisionCallback(Collision&);
	int curAnim;
	inline FVector2 GetToPlr() {
		return FVector2::FromTo(rb->newPosition, Player::GetPosition());
	}
	void ResetIfAttkFin(int animation);
public:
	SwordGuy();
	inline FVector2 GetPosition() {
		return rb->newPosition;
	}
};