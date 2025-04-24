#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class SoloLevel2 final : public Scene {
	public:
		SoloLevel2() : Scene("Solo2") {}

		bool Initialize(SDL_Renderer* renderer) override;


		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeTitle(SDL_Renderer* renderer);
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

