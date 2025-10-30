#pragma once
#include "enemy.hpp"
class Orc final: public Enemy {
private:
	void Update(void);
	Orc(CollisionCallback);
	enum OrcAnimation {
		idle,
		overhead,
		swing,
		death,
		dash,
		hurt,
		run,
		walk,
		jump,
		numOrcAnims,
	};
};