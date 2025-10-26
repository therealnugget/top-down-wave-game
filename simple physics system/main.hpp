#pragma once
#include "linkedList.hpp"
#include <iostream>
#include <SDL.h>
#include <tuple>
#define NUM_SIG_SCANKEYS SDL_SCANCODE_RALT
#define NUM_MOD_KEYS 3
#define LogErrArgs cout << "fucked: ";\
va_list ap;\
va_start(ap, num_args);\
for (int i = 0; i < num_args; i++) {\
	cout << va_arg(ap, char*);\
}\
cout << '\n'
using namespace std;
inline void Assert(bool condition, const char *errMessage) {
	if (condition) return;
	ThrowError(errMessage);
}
inline void Assert(bool condition, int num_args, ...) {
	if (condition) return;
	LogErrArgs;
	#ifdef DEBUG_BUILD
	throw new exception();
	#endif
}
inline void ThrowError(int num_args, ...) {
	LogErrArgs;
	#ifdef DEBUG_BUILD
	throw new exception();
	#endif
}
inline void ThrowError(const char *initErrMessage, const char *append) {
	cout << "error! broken: " << initErrMessage;
	cout << append << '\n';
	#ifdef DEBUG_BUILD
	throw new exception();
	#endif
}
inline void ThrowError(const char *initErrMessage) {
	cout << "error! broken: " << initErrMessage << '\n';
	#ifdef DEBUG_BUILD
	throw new exception();
	#endif
}
inline void CheckSDLError() {
	const char* error;
	if (!*(error = SDL_GetError())) return;
	ThrowError(error);
}