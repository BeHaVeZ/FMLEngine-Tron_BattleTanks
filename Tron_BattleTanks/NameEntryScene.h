#pragma once
#include "Scene.h"
#include "GameData.h"
#include "GameObject.h"
#include "TextComponent.h"
#include <array>

namespace FML
{
	class NameEntryScene : public Scene
	{
	public:
		bool Initialize(SDL_Renderer* renderer) override;
		void InitializeInput() override;
		void HandleInput(SDL_Event& event) override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

	private:
		std::string allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
		std::array<int, 3> charIndices = { 0, 0, 0 };
		int currentSlot{ 0 };

		int scoreToSubmit{ 0 };
		int highscoreCap{ 10 };

		void MoveUp();
		void MoveDown();
		void MoveLeft();
		void MoveRight();
		void SubmitScore();
		void InsertHighscoreSorted(const std::string& name);
		void UpdateArrowHighlight();
		std::vector<std::pair<std::string, int>> LoadHighscores();
		void SaveHighscores(const std::vector<std::pair<std::string, int>>& scores);

		SDL_Renderer* storedRenderer = nullptr;

		std::array<std::unique_ptr<GameObject>, 3> letterObjects;
		std::array<std::unique_ptr<GameObject>, 3> arrowUpObjects;
		std::array<std::unique_ptr<GameObject>, 3> arrowDownObjects;
		std::unique_ptr<GameObject> currentScoreText;
	};
}