#pragma once
#include <SDL.h>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include "usefulTypedefs.hpp"
class Images {
private:
	static std::unordered_map<const char*, SDL_Texture*> loadedTexs;
	static std::unordered_map<const char*, SDL_Surface*> loadedSurfaces;
	template<typename ValueType = SDL_Texture *>
	static inline void FinalizeMap(std::unordered_map<const char *, ValueType> map) {
		std::for_each(map.begin(), map.end(), [](std::pair<const char *, ValueType> pair) {
			free(pair.second);
			});
	}
public:
	static SDL_Texture* LoadTexture(std::string path);
	static SDL_Texture* LoadTexture(const char* path);
	static inline void Finalize() {
		FinalizeMap<SDL_Texture*>(loadedTexs);
		FinalizeMap<SDL_Surface*>(loadedSurfaces);
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
	int currentAnimation = 0, animFrameIndex = -1;
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