#pragma once
#include "enemy.hpp"
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
	float speed;
	void CollisionCallback(Collision&);
public:
	SwordGuy();
};