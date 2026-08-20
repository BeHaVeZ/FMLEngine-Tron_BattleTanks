#pragma once
#include "Scene.h"
#include <string>

namespace FML
{

	class CoopResultScene final : public Scene {
	public:
		CoopResultScene() : Scene("CoopResult") {}

		bool Initialize(SDL_Renderer* renderer) override;
		void HandleInput(SDL_Event& event) override;
		void InitializeInput() override;

	private:
		void InitializeTitle(SDL_Renderer* renderer);
		void InitializeStatsTable(SDL_Renderer* renderer);
		void InitializeSummary(SDL_Renderer* renderer);
		void InitializeSounds();

		void AddText(SDL_Renderer* renderer, const std::string& text, float x, float y, int fontSize, SDL_Color color);
		void AddRow(SDL_Renderer* renderer, const std::string& label, int player1Value, int player2Value, float y);

		void ReturnToMainMenu();

		static constexpr int titleFontSize = 32;
		static constexpr int rowFontSize = 20;
		static constexpr int summaryFontSize = 24;
		static constexpr int promptFontSize = 20;

		static constexpr float titleY = 90.f;
		static constexpr float headerY = 190.f;
		static constexpr float firstRowY = 240.f;
		static constexpr float rowSpacing = 44.f;
		static constexpr float totalY = 480.f;
		static constexpr float mvpY = 560.f;
		static constexpr float promptY = 650.f;

		static constexpr float labelX = 230.f;
		static constexpr float player1ColumnX = 560.f;
		static constexpr float player2ColumnX = 700.f;
	};

}
