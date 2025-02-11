#include "FPSComponent.h"
#include <iostream>
#include <string>

FPSComponent::FPSComponent(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, SDL_Color color)
    : storedRenderer(renderer), frameTimeAccumulator(0), frameCount(0), fpsCounter(0) {
    textComponent = new TextComponent("FPS 0", fontPath, fontSize, color, renderer);
}


FPSComponent::~FPSComponent() {
    delete textComponent;
}

void FPSComponent::Update(float deltaTime) {
    frameTimeAccumulator += deltaTime;
    frameCount++;

    if (frameTimeAccumulator >= 1.0f) {
        fpsCounter = frameCount / frameTimeAccumulator;
        frameCount = 0;
        frameTimeAccumulator = 0.0f;

        std::string fpsText = "FPS " + std::to_string(static_cast<int>(fpsCounter));
        textComponent->SetText(fpsText, storedRenderer);
    }
}


void FPSComponent::Render(SDL_Renderer* renderer) {
    textComponent->Render(renderer);
}
