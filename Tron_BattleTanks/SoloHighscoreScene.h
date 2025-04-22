#pragma once
#include "Scene.h"
#include <memory>
#include <vector>

namespace FML
{

	class SoloHighscoreScene : public Scene {
	public:
		SoloHighscoreScene() : storedRenderer(nullptr) {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void InitializeBackground(SDL_Renderer* renderer);

		void InitializeSounds();


	private:
		SDL_Renderer* storedRenderer;
	};

}