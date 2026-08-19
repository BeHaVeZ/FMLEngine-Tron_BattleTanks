#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>

namespace FML
{

	bool TextureManager::Load(const std::string& id, const std::string& filename, SDL_Renderer* renderer) {
		if (textureMap.find(id) != textureMap.end()) {
			return true;
		}

		SDL_Surface* surface = IMG_Load(filename.c_str());
		if (!surface) {
			std::cerr << "Failed to load surface from " << filename << " SDL_Error: " << IMG_GetError() << std::endl;
			return false;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		if (!texture) {
			std::cerr << "Failed to create texture from " << filename << " SDL_Error: " << SDL_GetError() << std::endl;
			return false;
		}

		textureMap[id] = texture;
		return true;
	}

	SDL_Texture* TextureManager::GetTexture(const std::string& id) {
		auto it = textureMap.find(id);
		return it != textureMap.end() ? it->second : nullptr;
	}

	void TextureManager::Clear() {
		for (auto& pair : textureMap) {
			SDL_DestroyTexture(pair.second);
		}
		textureMap.clear();
	}


}
