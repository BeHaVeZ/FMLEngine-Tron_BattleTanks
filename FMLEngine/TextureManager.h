#pragma once
#include "Singleton.h"
#include <SDL.h>
#include <string>
#include <map>

class TextureManager : public Singleton<TextureManager> {
public:
    bool Load(std::string id, std::string filename, SDL_Renderer* renderer);

    SDL_Texture* GetTexture(std::string id);

    void Clear();

private:
    friend class Singleton<TextureManager>;
    TextureManager() {}
    ~TextureManager() {
        Clear();
    }

    std::map<std::string, SDL_Texture*> textureMap;
};
