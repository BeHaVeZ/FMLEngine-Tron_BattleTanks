#pragma once
#include "Component.h"
#include "TextComponent.h"
#include <SDL_ttf.h>
#include <string>

namespace FML
{
	class FPSComponent : public Component {
	public:
		FPSComponent(SDL_Renderer* renderer);
		~FPSComponent();

		void Initialize() override;
		void Update(float deltaTime) override;

	private:
		TextComponent* textComponent;
		SDL_Renderer* storedRenderer;
		float fpsCounter;
		float frameTimeAccumulator;
		int frameCount;
	};
}