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
std::unordered_map<std::string, Images::ImageData*> Images::loadedImages;
bool Textures::InitAnim(Animator& anim, const char* basePath) {
	struct stat buffer;
	int i = 0;
	std::string iStr;
	//one for null-terminating character, one for underscore character length
	const size_t endBuffer = 2;
	const size_t pathLen = strlen(imagesPath.c_str()) + strlen(basePath) + strlen(bmp) + endBuffer;
	constexpr int maxIndexDigitLen = 10;//therefore max value of i is 9999999999
	char* filePath = new char[maxIndexDigitLen + pathLen];
	Animation* nextAnim = nullptr;
	const auto GetAnim = [&]() {
		if (nextAnim != nullptr) return nextAnim;
		return nextAnim = anim.LoadNextAnim();
		};
	auto FileExists = [&](const char* fileType) -> bool {
		bool fileFound = !stat((string(filePath) + fileType).c_str(), &buffer);
		if (fileFound) {
			strcat(filePath, fileType);
			return true;
		}
		return false;
		};
	auto basePathStr = string(basePath);
	std::unordered_map<std::string, Images::ImageData *>::const_iterator loadedImg;
	bool pastImgLoaded;
	bool isImgLoaded = false;
	Images::ImageData* data = nullptr;
	for(;;) {
		iStr = to_string(i);
		pastImgLoaded = isImgLoaded;
		strcpy(filePath, (imagesPath + basePath + '_' + iStr + '.').c_str());
		loadedImg = Images::GetImgLoaded((string(filePath) + bmp).c_str());
		if (loadedImg == Images::loadedImages.end()) loadedImg = Images::GetImgLoaded((string(filePath) + png).c_str());
		isImgLoaded = loadedImg != Images::loadedImages.end();
		if (!isImgLoaded && (pastImgLoaded && i != 0 || !FileExists(bmp) && !FileExists(png))) {
			delete[] filePath;
			if (i == 0) return false;
			GetAnim()->numOfFrames = i;
			return true;
		}
		if (isImgLoaded) data = loadedImg->second;
		GetAnim()->textures.push_back(Images::LoadTexture(filePath, isImgLoaded, data));
		i++;
	}
}
SDL_Texture *Images::LoadTexture(std::string path) {
	return LoadTexture(path.c_str());
}
SDL_Texture *Images::LoadTexture(const char* path, bool imgLoaded, Images::ImageData *data) {
	const char* errorType;
	SDL_Surface* loadedSurface;
	SDL_Texture* newTexture;
	if (!imgLoaded) {
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
		loadedSurface = data->surface;
		newTexture = data->texture;
	}
	//SDL_FreeSurface(loadedSurface);
	return newTexture;
}