#pragma once
#include "debug.hpp"
#include "SDL_ttf.h"
#include "physics.hpp"
#include "multicast delegates.hpp"
#include "timer.hpp"
#include "usefulTypedefs.hpp"
#include "PlayerProjectile.hpp"
#include <typeinfo>
class Text {
private:
	static TTF_Font* font;
	static constexpr int defaultTextSize = 100;
	static constexpr const char* fontPath = "fonts/Pixie.ttf";
	SDL_Surface *surface;
	SDL_Texture *texture;
	SDL_Rect* rect;
	Node<std::function<void(void)>>* updateNode;
	bool bIsPauseText;
	static constexpr SDL_Point pointOfRotation = {};
public:
	struct TextData {
	private:
		IntVec2 size;
		IntVec2 position;
		std::string text;
		bool bIsPauseText;
	public:
		TextData(){}
		TextData(IntVec2 _size, IntVec2 startPos, std::string textDisplay, bool isPauseText = false) : size(_size), position(startPos), text(textDisplay), bIsPauseText(isPauseText) {

		}
		inline void SetText(std::string newText) {
			text = newText;
		}
		friend class Text;
	};
protected:
	const SDL_Point *GetPointOfRotationAddr() {
		return &pointOfRotation;
	}
	inline void SetUpdateNode(Node<std::function<void(void)>>* node) {
		updateNode = node;
	}
	//assumes img is already initialized, etc.
	void Init(TextData* data) {
		rect = new SDL_Rect();
		bIsPauseText = data->bIsPauseText;
		if (!bIsPauseText) SetUpdateNode(Main::Updates += [this]() {Update(); });
		auto& size = data->size;
		size.IntoRectWH(rect);
		(data->position - size * .5f).IntoRectXY(rect);
		if (!font) {
			if (TTF_Init() == -1) ThrowError("ttf couldn't initialize: ", TTF_GetError());
			font = TTF_OpenFont(fontPath, defaultTextSize);
			if (!font) ThrowError(4, "couldn't load font at path ", fontPath, "error: ", TTF_GetError());
		}
		surface = TTF_RenderText_Solid_Wrapped(font, data->text.c_str(), SDL_Color(), 0);
		texture = SDL_CreateTextureFromSurface(Main::renderer, surface);
	}
public:
	Text(){}
	Text(TextData* data) {
		Init(data);
	}
	virtual ~Text() {
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);
		delete rect;
		if (!bIsPauseText) {
			Main::Updates -= updateNode;
			return;
		}
		Main::PauseUpdates -= updateNode;
	}
	void RenderText(void);
private:
	static void Finalize(void);
protected:
	virtual void Update(void);
};
class Item : public Text {
public:
	static constexpr int numItems = 3;
private:
	static int selectedItem;
	int itemIndex;
	bool firstItem;
	std::function<void(void)> onSelect;
	static constexpr IntVec2 itemTextSize = IntVec2(400, 175);
	static MouseBounds itemBounds[numItems];
	static constexpr IntVec2 ItemImgOffset = IntVec2 (-270, -20);
	static constexpr int itemYSeparation = 190;
	//don't worry about freeing. it doesn't actually belong to "Item", it's just stored in the image hashmap
	Textures::TextureRect ItemImg;
	//leave these in counting order so they can be indexed using GetRandInt(int, int).
	enum ItemType {
		maxHealthAdd,
		enemyTurner,
		poison,
		pickupRange,
		numItemTypes,
	};
	static constexpr bool itemCanRepeat[numItemTypes] = {
		true,
		false,
		false,
		true,
	};
	static std::vector<int> availableItemTypes;
	Node<std::function<void(void)>>* renderUpdateNode;
	static int itemTypes[numItems];
	static int itemIndices[numItems];
protected:
	inline void SetOnSelect(std::function<void(void)> selectFunc) {
		onSelect = selectFunc;
	}
	void Update(void) override;
public:
	static void StaticInit(void);
	static void MakeRandItem(int);
	Item(int index, const char *path, const char* message, IntVec2 ItemImgSize) {
		SetUpdateNode(Main::PauseUpdates += [this]() {Update(); });
		itemIndex = index;
		selectedItem = -1;
		firstItem = !index;
		auto pos = static_cast<IntVec2>(Main::halfDisplaySize) + IntVec2::GetUp() * (index - 1) * itemYSeparation;
		ItemImg = Textures::TextureRect(Textures::InitAnim(path), SDL_Rect{ pos.x + ItemImgOffset.x, pos.y + ItemImgOffset.y, ItemImgSize.x, ItemImgSize.y });
#ifdef DEBUG_BUILD
		if (!ItemImg.GetTexture()) ThrowError("couldn't load item with file path \"", path, "\"");
#endif
		auto data = Text::TextData(IntVec2::GetOne() * Item::itemTextSize, static_cast<FVector2>(pos), message, true);
		Init(&data);
	}
	virtual ~Item() {

	}
	friend class Main;
};
class MaxHealthAdd final : public Item {
private:
	void OnSelect(void);
	float healthIncrease;
public:
	MaxHealthAdd(int index) : healthIncrease(.04f), Item(index, "main/heart/heart", "increase max\nhealth by 4%", IntVec2(11 * 4, 9 * 4)) {
		SetOnSelect([this]() {OnSelect(); });
	}
};
class PickupRange final : public Item {
private:
	void OnSelect(void);
	float pickupIncrease;
public:
	PickupRange(int index) : pickupIncrease(.3f), Item(index, "increase_pickup_range", "increase pickup\nrange by 30%", IntVec2(11 * 4, 11 * 4)) {
		SetOnSelect([this]() {OnSelect(); });
	}
};
class EnemyTurner final : public Item {
private:
	void Update(void) override;
	static const std::string startPath;
	static const char *endPath;
	static const std::string fullPath;
	static constexpr int num_projectiles = 3;
public:
	EnemyTurner(int index) : Item(index, fullPath.c_str(), "turns enemies\nagainst one another", IntVec2(32 * 2, 32 * 2)) {
		SetOnSelect([]()-> void {
			new PlayerProjectile(startPath, endPath, num_projectiles);
			});
	}
};
class Poison final : public Item {
private:
	void Update(void) override;
	static const std::string startPath;
	static const char *endPath;
	static const std::string fullPath;
	static constexpr int num_projectiles = 6;
public:
	static constexpr int damageFrameWait = 60;
	static constexpr float damageAmount = .3f;
	Poison(int index) : Item(index, fullPath.c_str(), "poisons       \nenemies       ", IntVec2(32 * 2, 32 * 2)) {
		SetOnSelect([]()-> void {
			new PlayerProjectile(startPath, endPath, num_projectiles, Main::Tag::poison, 64);
			});
	}
};