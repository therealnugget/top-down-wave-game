#include "Item.hpp"
#include "player.hpp"
TTF_Font* Text::font = nullptr;
void Text::RenderText(void) {
	auto originPt = SDL_Point();
	SDL_RenderCopyEx(Main::renderer, texture, nullptr, rect, .0, &originPt, SDL_FLIP_NONE);
}
void Text::Update(void) {
	RenderText();
}
int Item::selectedItem = -1;
void Item::Update(void) {
	Text::Update();
	if (!Main::leftClickOnFrame) return;
	if (selectedItem == -1) {
		auto& mousePos = Main::mousePosition;
		for (int i = 0; i < numItems; i++) {
			if (Physics::PointInBounds(Main::mousePosition, itemBounds[i])) {
				selectedItem = i;
				break;
			}
		}
	}
	if (selectedItem == -1) return;
	if (selectedItem == itemIndex) onSelect();
	Main::canChangePause = true;
	Main::SetPauseState(false);
	delete this;
}
void Text::Finalize(void) {
	TTF_CloseFont(font);
}
void MaxHealthAdd::OnSelect(void) {
	Player::IncreaseHealth(healthIncrease);
}
void MaxHealthAdd::Update(void) {
	Item::Update();
}
void Item::MakeRandItem(int index) {
	switch (Main::GetRandInt(0, numItemTypes)) {
	case maxHealthAdd:
		new MaxHealthAdd(index);
		return;
	default:
		ThrowError("case not supported");
		return;
	}
}