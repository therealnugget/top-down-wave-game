#pragma once
#include "enemy.hpp"
class SwordGuy final: public Enemy {
private:
	Node<Entity *> *healthBar;
	Entity *healthBarEnt;
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
	static constexpr IntVec2 healthBarOffset = { -46, -106 };
public:
	SwordGuy();
};