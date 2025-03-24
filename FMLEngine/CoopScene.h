#pragma once
#include "Scene.h"
#include "GameObject.h"
#include <vector>
#include <memory>

namespace FML
{

	class CoopScene final : public Scene {
	public:
		bool Initialize(SDL_Renderer* renderer) override;


		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeTitle(SDL_Renderer* renderer);
		void InitializeFPSCounter(SDL_Renderer* renderer);
		void InitializeFirstTank();
		void InitializeSecondTank();
		void InitializeInput() override;

		void InitializeSounds();

		void HandleInput(SDL_Event& event) override;

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void Cleanup() override;
	};

}

