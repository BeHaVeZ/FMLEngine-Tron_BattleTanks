#include "NameEntryScene.h"
#include "FileReader.h"
#include "InputBindingHelper.h"
#include "TextComponent.h"
#include "SceneManager.h"
#include "Logger.h"
#include <sstream>
#include <fstream>

namespace FML
{
	bool NameEntryScene::Initialize(SDL_Renderer* renderer)
	{
		this->storedRenderer = renderer;
		scoreToSubmit = GameData::CurrentScore;

		InitializeInput();

		{
			auto gameOverTitle = std::make_unique<GameObject>("GameOverTitle");
			auto gameOverText = std::make_unique<TextComponent>("Game Over",
				"data/fonts/tron-arcade.ttf", 24, SDL_Color{ 0, 255, 255, 255 }, renderer);
			gameOverTitle->AddComponent(std::move(gameOverText));
			gameOverTitle->GetComponent<TransformComponent>()->SetPosition({ 370, 100 });
			AddGameObject(std::move(gameOverTitle));

			auto highscoreLabel = std::make_unique<GameObject>("HighScoreLabel");
			auto highscoreText = std::make_unique<TextComponent>("Highscore ",
				"data/fonts/tron-arcade.ttf", 24, SDL_Color{ 0, 255, 255, 255 }, renderer);
			highscoreLabel->AddComponent(std::move(highscoreText));
			highscoreLabel->GetComponent<TransformComponent>()->SetPosition({ 370, 150 });
			AddGameObject(std::move(highscoreLabel));


			auto scoreLabel = std::make_unique<GameObject>();
			auto scoreText = std::make_unique<TextComponent>("Your score " + std::to_string(scoreToSubmit),
				"data/fonts/tron-arcade.ttf", 24, SDL_Color{ 0, 255, 255, 255 }, renderer);
			scoreLabel->AddComponent(std::move(scoreText));
			scoreLabel->GetComponent<TransformComponent>()->SetPosition({ 370, 200 });
			AddGameObject(std::move(scoreLabel));
		}

		float baseX = 430.f;
		float spacing = 50.f;
		float letterY = 300.f;
		float arrowYUp = letterY - 50.f;
		float arrowYDown = letterY + 80.f;


		for (int i = 0; i < 3; ++i)
		{
			auto letterGO = std::make_unique<GameObject>(std::string("Letter") + std::to_string(i));
			auto letterText = std::make_unique<TextComponent>(std::string(1, allowedChars[charIndices[i]]),
				"data/fonts/tron-arcade.ttf", 48, SDL_Color{ 0, 255, 255, 255 }, renderer);
			letterGO->AddComponent(std::move(letterText));
			letterGO->GetComponent<TransformComponent>()->SetPosition({ baseX + i * spacing, letterY });
			letterObjects[i] = std::move(letterGO);
			AddGameObject(std::move(letterObjects[i]));

			auto upArrow = std::make_unique<GameObject>(std::string("UpArrow") + std::to_string(i));
			auto upArrowText = std::make_unique<TextComponent>("v", "data/fonts/tron-arcade.ttf", 24,
				SDL_Color{ 0, 255, 255, 255 }, renderer);
			upArrow->AddComponent(std::move(upArrowText));
			upArrow->GetComponent<TransformComponent>()->SetPosition({ baseX + i * spacing, arrowYUp });
			upArrow->GetComponent<TransformComponent>()->SetRotation(180);
			arrowUpObjects[i] = std::move(upArrow);
			AddGameObject(std::move(arrowUpObjects[i]));

			auto downArrow = std::make_unique<GameObject>(std::string("DownArrow") + std::to_string(i));
			auto downArrowText = std::make_unique<TextComponent>("v", "data/fonts/tron-arcade.ttf", 24,
				SDL_Color{ 0, 255, 255, 255 }, renderer);
			downArrow->AddComponent(std::move(downArrowText));
			downArrow->GetComponent<TransformComponent>()->SetPosition({ baseX + i * spacing, arrowYDown });
			arrowDownObjects[i] = std::move(downArrow);
			AddGameObject(std::move(arrowDownObjects[i]));
		}
		UpdateArrowHighlight();
		return true;
	}
	void NameEntryScene::InitializeInput()
	{
		int controllerId = 0;

		InputBindingHelper::BindGlobalCommands();

		InputHandler::Instance().BindFunction(SDLK_UP, [this]() { MoveUp(); },InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_DOWN, [this]() { MoveDown(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_LEFT, [this]() { MoveLeft(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_RIGHT, [this]() { MoveRight(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_RETURN, [this]() { SubmitScore(); }, InputHandler::KeyAction::KeyUp);

		InputHandler::Instance().BindGamepadFunction(controllerId, XINPUT_GAMEPAD_DPAD_UP, [this]() { MoveUp(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, [this]() { MoveDown(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, [this]() { MoveLeft(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, [this]() { MoveRight(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerId, XINPUT_GAMEPAD_A, [this]() { SubmitScore(); }, InputHandler::KeyAction::KeyUp);
	}
	void NameEntryScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}
	void NameEntryScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}
	void NameEntryScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}
	void NameEntryScene::MoveUp()
	{
		charIndices[currentSlot] = (charIndices[currentSlot] + 1) % allowedChars.size();
		auto* textComp = FindGameObjectByTag("Letter" + std::to_string(currentSlot))->GetComponent<TextComponent>();
		textComp->SetText(std::string(1, allowedChars[charIndices[currentSlot]]), SceneManager::Instance().GetRenderer());
	}

	void NameEntryScene::MoveDown()
	{
		charIndices[currentSlot] = (charIndices[currentSlot] - 1 + allowedChars.size()) % allowedChars.size();
		auto* textComp = FindGameObjectByTag("Letter" + std::to_string(currentSlot))->GetComponent<TextComponent>();
		textComp->SetText(std::string(1, allowedChars[charIndices[currentSlot]]), SceneManager::Instance().GetRenderer());
	}
	void NameEntryScene::MoveLeft()
	{
		if (currentSlot > 0) --currentSlot;
		UpdateArrowHighlight();
	}
	void NameEntryScene::MoveRight()
	{
		if (currentSlot < 2) ++currentSlot;
		UpdateArrowHighlight();
	}
	void NameEntryScene::SubmitScore()
	{
		if (currentSlot < 2) {
			MoveRight();
			return;
		}

		std::string name;
		for (int i : charIndices)
			name += allowedChars[i];

		InsertHighscoreSorted(name);
	}
	void NameEntryScene::InsertHighscoreSorted(const std::string& name)
	{
		auto scores = LoadHighscores();

		std::sort(scores.begin(), scores.end(), [](auto& a, auto& b)
			{
				return a.second > b.second;
			});

		if (scores.size() < highscoreCap || scoreToSubmit > scores.back().second)
		{
			scores.emplace_back(name, scoreToSubmit);

			std::sort(scores.begin(), scores.end(), [](auto& a, auto& b)
				{
					return a.second > b.second;
				});

			if (scores.size() > highscoreCap)
				scores.resize(highscoreCap);

			SaveHighscores(scores);
		}
		else
		{
			SceneManager::Instance().ChangeScene("SoloHighscore");
		}
	}

	void NameEntryScene::UpdateArrowHighlight()
	{
		for (int i = 0; i < 3; ++i)
		{
			SDL_Color highlight = { 0, 255, 255, 255 };
			SDL_Color dim = { 0, 100, 100, 255 };

			auto upArrowText = FindGameObjectByTag("UpArrow" + std::to_string(i))->GetComponent<TextComponent>();
			auto downArrowText = FindGameObjectByTag("DownArrow" + std::to_string(i))->GetComponent<TextComponent>();

			if (i == currentSlot)
			{
				upArrowText->SetColor(highlight, SceneManager::Instance().GetRenderer());
				downArrowText->SetColor(highlight, SceneManager::Instance().GetRenderer());
			}
			else
			{
				upArrowText->SetColor(dim, SceneManager::Instance().GetRenderer());
				downArrowText->SetColor(dim, SceneManager::Instance().GetRenderer());
			}
		}
	}
	std::vector<std::pair<std::string, int>> NameEntryScene::LoadHighscores()
	{
		FileReader reader("data/highscores.txt");
		auto lines = reader.readLines();
		std::vector<std::pair<std::string, int>> scores;

		for (const auto& line : lines) {
			std::istringstream iss(line);
			std::string name;
			int score;
			iss >> name >> score;
			scores.emplace_back(name, score);
		}

		return scores;
	}
	void NameEntryScene::SaveHighscores(const std::vector<std::pair<std::string, int>>& scores)
	{
		std::ofstream file("data/highscores.txt", std::ios::trunc);
		if (!file.is_open()) {
			Logger::Log(LogLevel::Error, "Failed to open highscores.txt for writing.");
			return;
		}

		for (const auto& [name, score] : scores)
			file << name << " " << score << "\n";

		//WRITE-THEN-READ immediately is avoideddd
		file.flush();
		file.close();

		SceneManager::Instance().ChangeScene("SoloHighscore");
	}
}