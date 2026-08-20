#pragma once
#include "Scene.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace FML
{

	class MainMenuScene : public Scene {
	public:
		MainMenuScene() : Scene("MainMenu"), storedRenderer(nullptr) {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

		void OnExit() override;

	private:
		enum class MenuPage
		{
			Root,
			Play,
			Settings
		};

		struct MenuItem
		{
			std::string label;
			SDL_Color color{ 0, 255, 255, 255 };
			std::function<void()> onActivate;
			std::function<std::string()> valueText;
			std::function<void(int)> onAdjust;
		};

		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeSounds();

		void QueuePage(MenuPage page);
		void BuildPage(MenuPage page);
		void BuildRootItems();
		void BuildPlayItems();
		void BuildSettingsItems();
		void SpawnItemObjects();
		void ClearItemObjects();

		void UpdateHighlight();
		void RefreshItemValue(size_t index);

		void MoveSelection(int delta);
		void Adjust(int direction);
		void Activate();
		void GoBack();

		void HandleMouseInput(const SDL_Event& event);
		bool FindItemAt(float x, float y, size_t& outIndex) const;

		std::vector<MenuItem> items;
		std::vector<GameObject*> itemObjects;
		std::vector<GameObject*> valueObjects;
		size_t selectedIndex = 0;

		MenuPage currentPage = MenuPage::Root;
		MenuPage pendingPage = MenuPage::Root;
		bool hasPendingPage = false;

		SDL_Renderer* storedRenderer;

		static constexpr int columnX = 300;
		static constexpr int valueColumnX = 660;
		static constexpr int firstRowY = 280;
		static constexpr int rowSpacing = 60;
		static constexpr int fontSize = 32;
		static constexpr Uint8 selectedAlpha = 255;
		static constexpr Uint8 dimmedAlpha = 100;
	};

}
