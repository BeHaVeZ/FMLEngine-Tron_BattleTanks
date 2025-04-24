#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class SoloScene final : public Scene {
	public:
		SoloScene() : Scene("Solo") {}

		bool Initialize(SDL_Renderer* renderer) override;


		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeFPSCounter(SDL_Renderer* renderer);
		void InitializeFirstTank();


		void InitializeUI();
		void InitializeHealthUI();
		void InitializeScoreUI();

		void InitializeInput() override;
		void InitializeSounds();

		void InitializeWalls();
		void InitializeManagers();

		void HandleInput(SDL_Event& event) override;

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void OnExit() override;
	};

}

