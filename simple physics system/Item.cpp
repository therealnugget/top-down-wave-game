#include "Item.hpp"
TTF_Font* Text::font = nullptr;
SDL_Surface* Text::surface = nullptr;
SDL_Texture* Text::texture = nullptr;
void Text::RenderText(Text* text) {
	auto originPt = SDL_Point();
	SDL_RenderCopyEx(Main::renderer, texture, nullptr, text->rect, .0, &originPt, SDL_FLIP_NONE);
}
void Text::Update(void) {
	RenderText(this);
	if (!Main::leftClickOnFrame) return;
	Main::canChangePause = true;
	Main::SetPauseState(false);
	delete this;
}
void Text::Finalize(void) {
	TTF_CloseFont(font);
}