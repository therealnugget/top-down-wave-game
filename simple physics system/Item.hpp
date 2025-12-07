#pragma once
#include "debug.hpp"
#include "SDL_ttf.h"
#include "physics.hpp"
#include "multicast delegates.hpp"
class Text {
public:
	static TTF_Font* font;
	static constexpr int defaultTextSize = 100;
	static SDL_Surface *surface;
	static SDL_Texture *texture;
	SDL_Rect* rect;
	Node<std::function<void(void)>>* updateNode;
	bool bIsPauseText;
	inline void SetUpdateNode(Node<std::function<void(void)>>* node) {
		updateNode = node;
	}
	//assumes img is already initialized, etc.
	Text(IntVec2 _size, IntVec2 startPos, std::string textDisplay, bool isPauseText = false): rect(new SDL_Rect()), bIsPauseText(isPauseText) {
		if (bIsPauseText) SetUpdateNode(Main::PauseUpdates += [this]() { Update(); });
		_size.IntoRectWH(rect);
		(startPos - _size * .5f).IntoRectXY(rect);
		if (!font) {
			auto path = "fonts/Pixie.ttf";
			if (TTF_Init() == -1) ThrowError("ttf couldn't initialize: ", TTF_GetError());
			font = TTF_OpenFont(path, defaultTextSize);
			if (!font) ThrowError(4, "couldn't load font at path ", path, "error: ", TTF_GetError());
			return;
		}
		if (surface) {
			SDL_FreeSurface(surface);
			SDL_DestroyTexture(texture);
		}
		surface = TTF_RenderText_Solid(font, textDisplay.c_str(), SDL_Color{ 0, 0, 0 });
		texture = SDL_CreateTextureFromSurface(Main::renderer, surface);
	}
	~Text() {
		delete rect;
		if (!bIsPauseText) return;
		Main::PauseUpdates -= updateNode;
	}
	static void RenderText(Text*);
	static void Finalize(void);
	void Update(void);
};