#pragma once
#include "Scene.h"

namespace FML
{

	class VersusResultScene final : public Scene {
	public:
		VersusResultScene() : Scene("VersusResult") {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;

	private:
		void InitializeResultText(SDL_Renderer* renderer);
		void InitializeSounds();

		void ReturnToMainMenu();

		static constexpr int titleFontSize = 32;
		static constexpr int promptFontSize = 20;
		static constexpr float titleY = 300.f;
		static constexpr float promptY = 400.f;
	};

}
