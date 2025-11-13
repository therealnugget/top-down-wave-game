#pragma once
#include "enemy.hpp"
class SwordGuy final: public Enemy {
private:
	void Update(void) override;
	enum SGAnimation {
		idle,
		overhead,
		swing,
		death,
		dash,
		hurt,
		run,
		walk,
		jump,
		numSGAnims,
	};
	float speed;
	void CollisionCallback(Collision&);
public:
	SwordGuy();
};