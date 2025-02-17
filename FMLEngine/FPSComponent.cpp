#include "FPSComponent.h"
#include <iostream>
#include <string>

FPSComponent::FPSComponent(SDL_Renderer* renderer)
	: storedRenderer(renderer), frameTimeAccumulator(0), frameCount(0), fpsCounter(0), textComponent(nullptr)
{
}

FPSComponent::~FPSComponent() 
{
}

void FPSComponent::Initialize()
{
	if (!gameObject)
	{
		std::cerr << "FPSComponent::Initialize() gameObject is nullptr" << std::endl;
		return;
	}
	if (!textComponent)
	{
		textComponent = gameObject->GetComponent<TextComponent>();
	}
	textComponent->SetText("FPS 0", storedRenderer);
}

void FPSComponent::Update(float deltaTime) 
{
	if (!textComponent) {
		return;
	}

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