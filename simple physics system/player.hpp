#pragma once
#include "main.hpp"
#include "physics.hpp"
#include <SDL.h>
//static
class Player : public Behaviour {
private:
	static enum direction {
		down = 0,
		right = 1,
		left = 2,
		up = 3,
		num_directions = 4,
	};
	static std::unordered_map<const char *, int> dirNames;
	static float accel, speed;
public:
	static void Init();
	static void Finalize();
	static void Update(void);
	friend class Main;
};