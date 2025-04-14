#pragma once
#include "Scene.h"
#include <memory>
#include <vector>

namespace FML
{

	class MainMenuScene : public Scene {
	public:
		MainMenuScene() : storedRenderer(nullptr), selectedIndex(0) {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeMenuOptions(SDL_Renderer* renderer);
		void InitializeSelectionArrow(SDL_Renderer* renderer);

		void InitializeSounds();


	private:
		std::vector<GameObject*> menuOptions;
		std::unique_ptr<GameObject> selectionArrow;
		int selectedIndex;

		SDL_Renderer* storedRenderer;
	};

}