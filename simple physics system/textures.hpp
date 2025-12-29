#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include "usefulTypedefs.hpp"
#include <tuple>
#include "main.hpp"
#include "debug.hpp"
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
private:
	bool bLoop = true;
	int numOfFrames;
	std::vector<SDL_Texture*> textures;
public:
	bool GetLoop() {
		return bLoop;
	}
	int GetNumFrames() const {
		return numOfFrames;
	}
	friend struct Entity;
	friend struct Animator;
	friend class Textures;
};
//why did i re-code this in this project after writing an animator in my c project? and for that matter, why did i write it again instead of copying the code? simple: to allow new ideas to propagate, which they did. to make sure i had it right the first time by having to create it from the ground up again and seeing if i find more efficient or effective ways of completing the same task.
struct Animator {
protected:
	//when currentAnimation is -1, the animator is not active.
	int currentAnimation = 0, pastAnimation = 0;
	bool bRecordAnim = true;
	//-1 is finished
	int animFrameIndex = -1;
	int numAnims = 0;
	float animSpeed = 1.f;
	float animTime = .0f;
	std::vector<Animation> anims;
private:
	inline Animation *LoadNextAnim(){
		anims.resize(static_cast<size_t>(numAnims + 1));
		return &anims[numAnims++];
	}
	friend class Textures;
public:
	constexpr static float default_anim_fps = 30.f;
	constexpr static float default_anim_time = 1.f / default_anim_fps;
	constexpr static float neg_anim_time = -default_anim_time;
	inline void DeActivate() {
		currentAnimation = -1;
	}
	inline void SetNotLoop(int anim) {
		anims[anim].bLoop = false;
	}
	inline void SetNotLoopDirs(int anim) {
		for (int i = 0; i < Main::num_directions; i++) {
			anims[anim + i].bLoop = false;
		}
	}
	inline void SetRecordAnim(bool value) {
		bRecordAnim = value;
	}
	const inline int GetNumAnims() const {
		return numAnims;
	}
	const inline int GetNumAnimFrames(int animation) const {
		return anims[animation].GetNumFrames();
	}
	inline void SetAnimSpd(float speed) {
		animSpeed = speed;
	}
	inline int GetNumAnimFrames() const {
		return anims[currentAnimation].GetNumFrames();
	}
	inline int GetCurAnim() {
		return currentAnimation;
	}
	inline bool AnimFinished(int animation = -1) {
		return animFrameIndex == -1 && (animation == -1 || GetCurAnim() == animation);
	}
	inline bool FirstFrame() {
		return animFrameIndex == 0;
	}
};
//static class
class Textures {
private:
	static rbList *curAnimNode;
	static RigidBody *curAnimRB;
public:
	//return value: whether an animation was actually created
	static SDL_Texture *InitAnim(const char *basePath, Animator *anim = nullptr);
	struct TextureRect {
	private:
		SDL_Texture* texture;
		SDL_Rect rect;
	public:
		inline SDL_Texture* GetTexture(void) {
			return texture;
		}
		inline SDL_Rect *GetRectAddr(void) {
			return &rect;
		}
		void SetPosition(IntVec2 position);
		TextureRect(SDL_Texture *_texture, SDL_Rect _rect) : texture(_texture), rect(_rect) {}
		TextureRect() {}
		~TextureRect() {}
	};
	static void RenderStandaloneTex(TextureRect&);
};