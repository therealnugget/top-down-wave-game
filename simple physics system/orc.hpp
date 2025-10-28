#pragma once
class Orc {
private:
	Orc();
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