#include "Item.hpp"
#include "player.hpp"
TTF_Font* Text::font = nullptr;
void Text::RenderText(void) {
	SDL_RenderCopyEx(Main::renderer, texture, nullptr, rect, .0, GetPointOfRotationAddr(), SDL_FLIP_NONE);
}
void Text::Update(void) {
	RenderText();
}
int Item::selectedItem = -1;
void Item::Update(void) {
	Text::Update();
	Textures::RenderStandaloneTex(ItemImg);
	if (!Main::leftClickOnFrame) return;
	if (selectedItem == -1) {
		for (int i = 0; i < numItems; i++) {
			if (Physics::PointInBounds(Main::rawMousePosition, itemBounds[i])) {
				selectedItem = i;
				goto selected;
			}
		}
		return;
	}
selected:
	if (selectedItem == itemIndex && onSelect) {
		onSelect();
	}
	Main::canChangePause = true;
	Main::SetPauseState(false);
	delete this;
}
void Text::Finalize(void) {
	TTF_CloseFont(font);
}
void MaxHealthAdd::OnSelect(void) {
	Player::IncreaseHealth(healthIncrease);
	Player::ReplenishHealth();
}
bool EnemyTurner::playerCollected = false;
void EnemyTurner::Update(void) {
	Item::Update();
}
unordered_set<int> Item::itemTypes;
void Item::StaticInit(void) {
	itemTypes.reserve(numItemTypes);
	for (int i = numItemTypes - 1; i >= 0; i--) itemTypes.insert(i);
}
void Item::MakeRandItem(int index) {
	auto randInd = Main::GetRandInt(0, itemTypes.size());
	int i = 0;
	for (auto itemType : itemTypes) if (i++ == randInd) {
		switch (itemType) {
		case maxHealthAdd:
			new MaxHealthAdd(index);
			break;
		case enemyTurner:
			new EnemyTurner(index);
			break;
		default:
			ThrowError("case not supported");
			break;
		}
		if (!itemCanRepeat[itemType]) itemTypes.erase(itemType);
		return;
	}
}