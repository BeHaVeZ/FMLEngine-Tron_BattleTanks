#pragma once
#include "Scene.h"
#include <memory>
#include <vector>

namespace FML
{

	class MainMenuScene : public Scene {
	public:
		MainMenuScene() : Scene("MainMenu"), 
			storedRenderer(nullptr), selectedOption(nullptr) {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeMenuOptions(SDL_Renderer* renderer);
		void UpdateMenuOptionHighlight();

		void SelectPlay();
		void SelectQuit();
		void SelectCoop();
		void SelectVersus();

		void ExecuteMenuOption();

		void InitializeSounds();

		void OnExit() override;

	private:
		std::vector<GameObject*> menuOptions;
		GameObject* selectedOption = nullptr;

		SDL_Renderer* storedRenderer;
	};

}