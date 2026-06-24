#pragma once
#include "main.hpp"
struct Timer {
public:
	Timer(bool useTimeScale = false): pastTime(SDL_GetPerformanceCounter()), bUseTimeScale(useTimeScale) {}
	Timer(float startSec, bool useTimeScale = false): pastTime(SDL_GetPerformanceCounter() - static_cast<Uint64>(Main::frequency * startSec)), bUseTimeScale(useTimeScale){}
	//this is also initialization for when you can't initialize because you're allocating on the stack
	inline void Reset(float startSec = .0f, bool useTimeScale = true) {
		pastTime = SDL_GetPerformanceCounter() - static_cast<Uint64>(Main::frequency * startSec);
		bUseTimeScale = useTimeScale;
	}
	inline float GetElapsedSeconds() {
		return static_cast<float>(SDL_GetPerformanceCounter() - pastTime) / Main::frequency * (Main::timeScale * bUseTimeScale + !bUseTimeScale);
	}
	inline void PrintElapsedSeconds(const char *initMessage = Main::empty_cc) {
		std::cout << initMessage << ' ' << GetElapsedSeconds() << std::endl;
	}
private:
	Uint64 pastTime;
	bool bUseTimeScale;
};