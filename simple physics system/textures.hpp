#pragma once
#include <SDL.h>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include "usefulTypedefs.hpp"
#include <tuple>
class Images {
public:
	struct ImageData {
	private:
		SDL_Surface *surface;
		SDL_Texture *texture;
		friend class Images;
	public:
		ImageData(SDL_Surface *_surface, SDL_Texture *_texture): surface(_surface), texture(_texture) {}
	};
	static std::unordered_map<std::string, ImageData*> loadedImages;
	template<typename first, typename second>
	static inline void FinalizeMap(std::unordered_map<std::string, std::tuple<first, second>> map) {
		std::for_each(map.begin(), map.end(), [](std::pair<std::string, std::tuple<first, second>> pair) {
			free(std::get<first>(pair.second));
			free(std::get<second>(pair.second));
			});
	}
	static std::unordered_map<std::string, ImageData *>::const_iterator GetImgLoaded(std::string path) {
		return loadedImages.find(path);
	}
	static SDL_Texture* LoadTexture(std::string path);
	static SDL_Texture* LoadTexture(const char* path, bool, ImageData *);
	static inline void Finalize() {
		for (auto &keyVal: loadedImages){
			auto& val = keyVal.second;
			free(val->surface);
			free(val->texture);
			};
	}
};
class Textures;
struct Animation {
protected:
	int numOfFrames;
	std::vector<SDL_Texture*> textures;
private:
	friend struct Entity;
	friend class Textures;
};
struct Animator {
protected:
	int currentAnimation = 0, pastAnimation = 0;
	bool bRecordAnim = false;
	int animFrameIndex = -1;
	int numAnims = 0;
	float animTime = .0f;
	std::vector<Animation> anims;
private:
	inline Animation *LoadNextAnim(){
		anims.resize(static_cast<size_t>(numAnims + 1));
		return &anims[numAnims++];
	}
	friend class Textures;
public:
	constexpr static float default_anim_time = 1.f / 12.f;
	constexpr static float neg_anim_time = -default_anim_time;
	inline void SetAnimation(int anim) {
		currentAnimation = anim;
		if (pastAnimation == anim) return;
		animFrameIndex = -1;
		animTime = .0f;
	}
	inline void SetRecordAnim(bool value) {
		bRecordAnim = value;
	}
	const inline int GetNumAnims() const {
		return numAnims;
	}
};
//static class
class Textures {
private:
	static rbList *curAnimNode;
	static RigidBody *curAnimRB;
public:
	//return value: whether an animation was actually created
	static bool InitAnim(Animator &animator, const char *basePath);
};