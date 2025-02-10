#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>

bool TextureManager::Load(std::string id, std::string filename, SDL_Renderer* renderer) {
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

SDL_Texture* TextureManager::GetTexture(std::string id) {
    return textureMap[id];
}

void TextureManager::Clear() {
    for (auto& pair : textureMap) {
        SDL_DestroyTexture(pair.second);
    }
    textureMap.clear();
}
