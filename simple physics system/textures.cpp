#include "textures.hpp"
#include "debug.hpp"
#include "winMgr.hpp"
#include <sys/stat.h>
#include "player.hpp"
#include "physics.hpp"
static const std::string imagesPath = "images/";
static const char *bmp = ".bmp";
rbList *Textures::curAnimNode = nullptr;
RigidBody *Textures::curAnimRB = nullptr;
bool Textures::InitAnim(Animator& anim, const char* basePath) {
	struct stat buffer;
	char *filePath;
	int i = 0;
	std::string iStr;
	//one for null-terminating character, one for underscore character length
	const size_t endBuffer = 2;
	const size_t pathLen = strlen(imagesPath.c_str()) + strlen(basePath) + strlen(bmp) + endBuffer;
	Animation* nextAnim = nullptr;
	const auto GetAnim = [&]() {
		if (nextAnim != nullptr) return nextAnim;
		return nextAnim = anim.LoadNextAnim();
		};
	for(;;) {
		iStr = to_string(i);
		filePath = new char[strlen(iStr.c_str()) + pathLen];
		strcpy(filePath, (imagesPath + basePath + '_' + iStr + bmp).c_str());
		if (stat(filePath, &buffer)) {
			if (i == 0) return false;
			GetAnim()->numOfFrames = i;
			return true;
		}
		GetAnim()->textures.push_back(Images::LoadTexture(filePath));
		i++;
	}
}
SDL_Texture *Images::LoadTexture(std::string path) {
	return LoadTexture(path.c_str());
}
SDL_Texture *Images::LoadTexture(const char* path) {
	auto throwImgErr = [&](const char* problem) {
		ThrowError(5, "couldn't load ", problem, " with path \"", path, "\". Error: ", SDL_GetError());
	};
	SDL_Surface* loadedSurface;
	std::unordered_map<const char*, SDL_Surface *>::const_iterator iter = loadedSurfaces.find(path);
	if (iter == loadedSurfaces.end()) {
		loadedSurface = SDL_LoadBMP(path);
		loadedSurfaces.emplace(path, loadedSurface);
		if (!loadedSurface) {
		err:
			throwImgErr("surface");
			return nullptr;
		}
	}
	else loadedSurface = iter->second;
	SDL_Texture* newTexture;
	std::unordered_map<const char*, SDL_Texture*>::const_iterator texIter = loadedTexs.find(path);
	if (texIter == loadedTexs.end()) {
		newTexture = SDL_CreateTextureFromSurface(Main::renderer, loadedSurface);
		loadedTexs.emplace(path, newTexture);
		if (!newTexture) {
			throwImgErr("texture");
			return nullptr;
		}
	}
	else {
		newTexture = texIter->second;
	}
	//SDL_FreeSurface(loadedSurface);
	return newTexture;
}