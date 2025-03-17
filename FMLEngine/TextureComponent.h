#pragma once
#include <SDL.h>
#include <string>
#include "Component.h"
#include <glm.hpp>

class TextureComponent : public Component {
public:
    TextureComponent(const std::string& filePath, SDL_Renderer* renderer);
    void Render(SDL_Renderer* renderer) override;

    void OffsetPivotPoint(glm::vec2 offset);

private:
    SDL_Texture* texture;
    SDL_Rect destRect;

    SDL_Point pivotPoint;

    int defaultWidth;
    int defaultHeight;
};
