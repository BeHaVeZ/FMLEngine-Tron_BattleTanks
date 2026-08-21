#pragma once
#include <SDL.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"

namespace FML
{
	class PauseMenu
	{
	public:
		static PauseMenu& Instance();

		void Toggle();
		void Open();
		void Close();
		bool IsOpen() const { return isOpen; }

		void MoveSelection(int delta);
		void Activate();

		void Render(SDL_Renderer* renderer);

		PauseMenu(const PauseMenu&) = delete;
		PauseMenu& operator=(const PauseMenu&) = delete;

	private:
		PauseMenu() = default;

		struct Item
		{
			std::string label;
			std::function<void()> onActivate;
		};

		void BuildItems();
		void SpawnItemObjects(SDL_Renderer* renderer);
		void UpdateHighlight();

		std::vector<Item> items;
		std::vector<std::unique_ptr<GameObject>> itemObjects;
		std::unique_ptr<GameObject> titleObject;
		SDL_Rect panelRect{};
		size_t selectedIndex = 0;
		bool isOpen = false;

		static constexpr int fontSize = 32;
		static constexpr int titleFontSize = 48;
		static constexpr int rowSpacing = 60;
		static constexpr int titleToFirstRow = 110;
		static constexpr Uint8 selectedAlpha = 255;
		static constexpr Uint8 dimmedAlpha = 100;
		static constexpr Uint8 scrimAlpha = 170;
		static constexpr Uint8 panelAlpha = 235;
		static constexpr int panelPadding = 40;
		static constexpr float musicDuck = .25f;
	};
}
