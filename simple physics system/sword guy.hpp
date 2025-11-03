#pragma once
#include "enemy.hpp"
class Orc final: public Enemy {
private:
	void Update(void) override;
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
public:
	Orc();
};