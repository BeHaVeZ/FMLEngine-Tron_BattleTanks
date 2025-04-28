#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class TestingScene final : public Scene {
	public:
		TestingScene() : Scene("Testing") {}
		bool Initialize(SDL_Renderer* renderer) override;


		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeFPSCounter(SDL_Renderer* renderer);
		void InitializeFirstTank();
		void InitializeWalls();

		void InitializeInput() override;
		void InitializeSounds();

		void HandleInput(SDL_Event& event) override;

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;
	};

}

