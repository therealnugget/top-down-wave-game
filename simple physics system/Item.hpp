#pragma once
#include "debug.hpp"
#include "SDL_ttf.h"
#include "physics.hpp"
#include "multicast delegates.hpp"
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
public:
	struct TextData {
	private:
		IntVec2 size;
		IntVec2 position;
		const char* text;
		bool bIsPauseText;
	public:
		TextData(){}
		TextData(IntVec2 _size, IntVec2 startPos, const char* textDisplay, bool isPauseText = false) : size(_size), position(startPos), text(textDisplay), bIsPauseText(isPauseText) {

		}
		inline void SetText(const char* newText) {
			text = newText;
		}
		friend class Text;
	};
protected:
	inline void SetUpdateNode(Node<std::function<void(void)>>* node) {
		updateNode = node;
	}
	//assumes img is already initialized, etc.
	void Init(TextData* data) {
		rect = new SDL_Rect();
		bIsPauseText = data->bIsPauseText;
		auto& size = data->size;
		size.IntoRectWH(rect);
		(data->position - size * .5f).IntoRectXY(rect);
		if (!font) {
			if (TTF_Init() == -1) ThrowError("ttf couldn't initialize: ", TTF_GetError());
			font = TTF_OpenFont(fontPath, defaultTextSize);
			if (!font) ThrowError(4, "couldn't load font at path ", fontPath, "error: ", TTF_GetError());
		}
		surface = TTF_RenderText_Solid_Wrapped(font, data->text, SDL_Color(), 0);
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
		if (!bIsPauseText) return;
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
protected:
	void SetOnSelect(std::function<void(void)> selectFunc) {
		onSelect = selectFunc;
	}
	void Update(void) override;
	static constexpr IntVec2 itemTextSize = IntVec2(400, 175);
	static constexpr MouseBounds itemBounds[numItems] = { {{756, 643}, {1165, 807}}, {{756, 458}, {1165, 643}}, {{756, 274}, {1165, 458}} };
	static constexpr int itemYSeparation = 190;
	enum ItemType {
		maxHealthAdd,
		numItemTypes,
	};
public:
	static void MakeRandItem(int);
	Item(int index, const char* message) {
		itemIndex = index;
		selectedItem = -1;
		firstItem = !index;
		auto data = Text::TextData(IntVec2::GetOne() * Item::itemTextSize, Main::halfDisplaySize + IntVec2::GetUp() * (index - 1) * itemYSeparation, message, true);
		Init(&data);
	}
};
class MaxHealthAdd final : public Item {
private:
	void OnSelect(void);
	float healthIncrease;
protected:
	void Update(void) override;
public:
	MaxHealthAdd(int index) : healthIncrease(.1f), Item(index, "increase max\nhealth by 10%") {
		SetOnSelect([this]() {OnSelect(); });
		SetUpdateNode(Main::PauseUpdates += [this]() { Update(); });
	}
};