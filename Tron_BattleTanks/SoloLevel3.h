#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class SoloLevel3 final : public Scene {
	public:
		SoloLevel3() : Scene("Solo3") {}
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
		void InitializeManagers();
		void InitializeCenterTP();

		void InitializeWalls();

		void HandleInput(SDL_Event& event) override;

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void OnExit() override;
	};
}