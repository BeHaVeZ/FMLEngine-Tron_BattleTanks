#pragma once
#include "Component.h"
#include "TextComponent.h"
#include <SDL_ttf.h>
#include <string>

class FPSComponent : public Component {
public:
    FPSComponent(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, SDL_Color color);
    ~FPSComponent();

    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;

private:
    TextComponent* textComponent;
    SDL_Renderer* storedRenderer;
    float fpsCounter;
    float frameTimeAccumulator;
    int frameCount;
};
