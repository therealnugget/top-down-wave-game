#pragma once
#include "main.hpp"
#include "physics.hpp"
#include <SDL.h>
//static
class Player : public Behaviour {
private:
	//even though this is guaranteed to have a size equivalent to "num_directions", for readability and the ability to have the directions in any order, it must be a dict.
	static std::unordered_map<int, const char *> dirNames;
	static float accel, speed;
public:
	static void Init();
	static void Finalize();
	static void Update(void);
	friend class Main;
};