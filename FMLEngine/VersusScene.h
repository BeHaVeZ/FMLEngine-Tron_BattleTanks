#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class VersusScene final : public Scene {
	public:
		VersusScene() : Scene("Versus") {}
		bool Initialize(SDL_Renderer* renderer) override;


		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeFPSCounter(SDL_Renderer* renderer);
		void InitializeFirstTank();
		void InitializeSecondTank();
		void InitializeInput() override;


		void InitializeUI();
		void InitializeHealthUI();
		void InitializeScoreUI();
		void InitializeCenterTP();

		void InitializeWalls();


		void InitializeSounds();

		void HandleInput(SDL_Event& event) override;

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void OnExit() override;

	private:
		static constexpr int hudHeight = 100;
		static constexpr int navCellSize = 8;
	};

}


