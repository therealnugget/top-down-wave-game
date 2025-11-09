#pragma once
#include "main.hpp"
struct Timer {
public:
	Timer(): pastTime(SDL_GetPerformanceCounter()){}
	//doesn't account for timeScale.
	inline float GetElapsedSeconds() {
		return static_cast<float>(SDL_GetPerformanceCounter() - pastTime) / Main::frequency;
	}
	inline void PrintElapsedSeconds(const char *initMessage = Main::empty_cc) {
		std::cout << initMessage << ' ' << GetElapsedSeconds() << std::endl;
	}
private:
	Uint64 pastTime;
};