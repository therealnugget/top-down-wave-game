#pragma once
#include "enemy.hpp"
class Orc : public Enemy {
private:
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