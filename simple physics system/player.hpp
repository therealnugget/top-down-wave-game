#pragma once
#include "main.hpp"
#include "physics.hpp"
#include <SDL.h>
//static
class Player : public Behaviour {
private:
	static float accel, speed;
	static IntVec2 pastInp;
	static void PlayAnim(int);
public:
	static void Init();
	static void Finalize();
	static void Update(void);
	friend class Main;
	static enum PlrAnim {
		idle,
		run,

		numAnims,
	};
};