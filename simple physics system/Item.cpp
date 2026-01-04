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
int Item::itemTypes[Item::numItems];
int Item::itemIndices[Item::numItems];
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
		if (!itemCanRepeat[itemTypes[itemIndex]]) {
			for (int i = itemIndices[itemIndex]; i < availableItemTypes.size() - 1; i++) {
				availableItemTypes[i] = availableItemTypes[i + 1];
			}
			availableItemTypes.resize(availableItemTypes.size() - 1);
		}
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
void PickupRange::OnSelect(void) {
	Player::IncreasePickupRange(pickupIncrease);
}
const std::string EnemyTurner::startPath = "question mark";
const char *EnemyTurner::endPath = "question mark";
const std::string EnemyTurner::fullPath = EnemyTurner::startPath + '/' + EnemyTurner::endPath;
const std::string Poison::startPath = "poison";
const char *Poison::endPath = "poison";
const std::string Poison::fullPath = Poison::startPath + '/' + Poison::endPath;
void EnemyTurner::Update(void) {
	Item::Update();
}
void Poison::Update(void) {
	Item::Update();
}
vector<int> Item::availableItemTypes;
void Item::StaticInit(void) {
	availableItemTypes.resize(numItemTypes);
	for (int i = 0; i < numItemTypes; i++) availableItemTypes[i] = i;
}
//functions like these, and other parts of the code, are incredibly repetitive. i could use parameterized definitions to cut down the maintainability and character count of the code, but the problem with parameterized definitions is that they destroy the readability of the code. it is never easy to understand what is happening inside the definition. for this reason, i stopped using them a while ago.
void Item::MakeRandItem(int index) {
	auto randInd = Main::GetRandInt(0, availableItemTypes.size());
	auto randType = availableItemTypes[randInd];
	switch (randType) {
	case maxHealthAdd:
		new MaxHealthAdd(index);
		break;
	case enemyTurner:
		new EnemyTurner(index);
		break;
	case poison:
		new Poison(index);
		break;
	case pickupRange:
		new PickupRange(index);
		break;
	default:
		ThrowError("case not supported");
		break;
	}
	itemTypes[index] = randType;
	itemIndices[index] = randInd;
}