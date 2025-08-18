#pragma once
#include "linkedList.hpp"
#ifdef DEBUG_BUILD
#include <Windows.h>
#endif
#include <iostream>
#include <SDL.h>
#include <tuple>
#define NUM_SIG_SCANKEYS SDL_SCANCODE_RALT
#define NUM_MOD_KEYS 3
using namespace std;
inline void ThrowError(int num_args, ...) {
	cout << "fucked: ";
	va_list ap;
	va_start(ap, num_args);
	for (int i = 0; i < num_args; i++) {
		cout << va_arg(ap, char*);
	}
	cout << '\n';
	#ifdef DEBUG_BUILD
	DebugBreak();
	#endif
}
inline void ThrowError(const char *initErrMessage, const char *append) {
	cout << "error! fucked: " << initErrMessage;
	cout << append << '\n';
	#ifdef DEBUG_BUILD
	DebugBreak();
	#endif
}
inline void ThrowError(const char *initErrMessage) {
	cout << "error! fucked: " << initErrMessage << '\n';
	#ifdef DEBUG_BUILD
	DebugBreak();
	#endif
}
inline void ThrowError() {
	const char* temp;
	if (!*(temp = SDL_GetError())) return;
	ThrowError(temp);
}