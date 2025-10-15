#pragma once
#include "main.hpp"
#include "physics.hpp"
#include <SDL.h>
//static, therefore can't inherit from behaviour.
class Player {
private:
	static rbList* temp;
	static float accel, speed;
	static IntVec2 pastInp;
	static void PlayAnim(int);
	static std::initializer_list<const char*> anims;
	static std::unordered_map<int, const char*> animNames;
	//even though this is guaranteed to have a size equivalent to "num_directions", for readability and the ability to have the directions in any order, it must be a dict.
	static std::unordered_map<int, const char*> dirNames;
	static inline void SetPastInp(void (*delMov)() = nullptr, void (*delIdle)() = nullptr) {
		if (Main::fInputVec == FVector2::Zero) {
			if (!delIdle) return;
			delIdle();
			return;
		}
		pastInp = Main::iInputVec;
		if (!delMov) return;
		delMov();
	}
public:
	static void Init(void);
	static void Finalize(void);
	static void Update(void);
	friend class Main;
	static enum PlrAnim {
		idle,
		run,
		attack,
		numAnims,
	};
};