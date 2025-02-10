#pragma once
#include <SDL.h>
#include <string>
#include "Component.h"

class TextureComponent : public Component {
public:
    TextureComponent(const std::string& filePath, SDL_Renderer* renderer);
    void Render(SDL_Renderer* renderer) override;

private:
    SDL_Texture* texture;
    SDL_Rect destRect;

    int defaultWidth;
    int defaultHeight;
};
