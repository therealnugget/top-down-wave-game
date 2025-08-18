#pragma once
#include <SDL.h>
//no constructor because this class should be marked with the "static" keyword, however you can't do that in c++, only c#.
class WindowManager {
public:
    static SDL_Window* window;
    static SDL_Surface* screenSurface;
};