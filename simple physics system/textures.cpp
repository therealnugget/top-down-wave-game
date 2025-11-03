#include "textures.hpp"
#include "debug.hpp"
#include "winMgr.hpp"
#include <sys/stat.h>
#include "player.hpp"
#include "physics.hpp"
#include <SDL_image.h>
static const std::string imagesPath = "images/";
static const char *bmp = "bmp";
static const char *png = "png";
rbList *Textures::curAnimNode = nullptr;
RigidBody *Textures::curAnimRB = nullptr;
std::unordered_map<const char*, Images::ImageData*> Images::loadedImages;
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
	auto FileExists = [&](const char* fileType) -> bool {
		strcpy(filePath, (imagesPath + basePath + '_' + iStr + '.' + fileType).c_str());
		return !stat(filePath, &buffer);
		};
	for(;;) {
		iStr = to_string(i);
		filePath = new char[strlen(iStr.c_str()) + pathLen];
		if (!FileExists(bmp) && !FileExists(png)) {
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
	const char* errorType;
	SDL_Surface* loadedSurface;
	auto iter = loadedImages.find(path);
	SDL_Texture* newTexture;
	if (iter == loadedImages.end()) {
		loadedSurface = SDL_LoadBMP(path);
		if (!loadedSurface) {
			loadedSurface = IMG_Load(path);
			if (!loadedSurface) {
				errorType = "image load";
				goto err;
			}
			loadedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
			if (!loadedSurface) {
				errorType = "convert surface";
				goto err;
			}
			goto loadTex;
		}
		goto loadTex;
	err:
		ThrowError(5, "couldn't load ", errorType, " with path \"", path, "\". Error: ", SDL_GetError());
		return nullptr;
	loadTex:
		newTexture = SDL_CreateTextureFromSurface(Main::renderer, loadedSurface);
		SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
		loadedImages.emplace(path, new ImageData(loadedSurface, newTexture));
		if (!newTexture) {
			errorType = "texture";
			goto err;
		}
	}
	else {
		auto loadedVal = iter->second;
		loadedSurface = loadedVal->surface;
		newTexture = loadedVal->texture;
	}
	//SDL_FreeSurface(loadedSurface);
	return newTexture;
}