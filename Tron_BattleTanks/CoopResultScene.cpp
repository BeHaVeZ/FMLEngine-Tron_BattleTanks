#include "CoopResultScene.h"
#include "ConfigManager.h"
#include "GameData.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include <memory>
#include <string>

namespace FML
{
	namespace
	{
		float TextWidth(size_t characters, int fontSize)
		{
			return characters * (fontSize * 0.875f);
		}

		float CentredX(size_t characters, int fontSize)
		{
			return (ConfigManager::Instance().GetWindowWidth() - TextWidth(characters, fontSize)) * 0.5f;
		}

		const char* Player2Name()
		{
			return GameData::Player2IsAI ? "AI" : "P2";
		}

		std::string MvpText()
		{
			const int player1Score = GameData::Player1Stats.score;
			const int player2Score = GameData::Player2Stats.score;

			if (player1Score == player2Score)
				return "TIED";
			if (player1Score > player2Score)
				return "PLAYER 1 MVP";

			return GameData::Player2IsAI ? "AI MVP" : "PLAYER 2 MVP";
		}
	}

	bool CoopResultScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeTitle(renderer);
		InitializeStatsTable(renderer);
		InitializeSummary(renderer);
		InitializeInput();
		InitializeSounds();

		return true;
	}

	void CoopResultScene::AddText(SDL_Renderer* renderer, const std::string& text, float x, float y, int fontSize, SDL_Color color)
	{
		auto textObject = std::make_unique<GameObject>();
		textObject->AddComponent(std::make_unique<TextComponent>(
			text, "data/fonts/tron-arcade.ttf", fontSize, color, renderer));
		textObject->GetComponent<TransformComponent>()->SetPosition({ x, y });
		AddGameObject(std::move(textObject));
	}

	void CoopResultScene::AddRow(SDL_Renderer* renderer, const std::string& label, int player1Value, int player2Value, float y)
	{
		const SDL_Color labelColor{ 0, 255, 0, 255 };
		const SDL_Color player1Color{ 0, 128, 255, 255 };
		const SDL_Color player2Color{ 255, 0, 0, 255 };

		AddText(renderer, label, labelX, y, rowFontSize, labelColor);

		const std::string player1Text = std::to_string(player1Value);
		const std::string player2Text = std::to_string(player2Value);

		AddText(renderer, player1Text, player1ColumnX - TextWidth(player1Text.size(), rowFontSize), y, rowFontSize, player1Color);
		AddText(renderer, player2Text, player2ColumnX - TextWidth(player2Text.size(), rowFontSize), y, rowFontSize, player2Color);
	}

	void CoopResultScene::InitializeTitle(SDL_Renderer* renderer)
	{
		const std::string title = "COOP RESULTS";
		AddText(renderer, title, CentredX(title.size(), titleFontSize), titleY, titleFontSize, SDL_Color{ 0, 255, 0, 255 });
	}

	void CoopResultScene::InitializeStatsTable(SDL_Renderer* renderer)
	{
		const std::string player1Header = "P1";
		const std::string player2Header = Player2Name();

		AddText(renderer, player1Header, player1ColumnX - TextWidth(player1Header.size(), rowFontSize), headerY, rowFontSize, SDL_Color{ 0, 128, 255, 255 });
		AddText(renderer, player2Header, player2ColumnX - TextWidth(player2Header.size(), rowFontSize), headerY, rowFontSize, SDL_Color{ 255, 0, 0, 255 });

		const GameData::PlayerStats& player1 = GameData::Player1Stats;
		const GameData::PlayerStats& player2 = GameData::Player2Stats;

		AddRow(renderer, "BLUE TANKS", player1.blueTankKills, player2.blueTankKills, firstRowY);
		AddRow(renderer, "PINK TANKS", player1.pinkTankKills, player2.pinkTankKills, firstRowY + rowSpacing);
		AddRow(renderer, "RECOGNIZERS", player1.recognizerKills, player2.recognizerKills, firstRowY + rowSpacing * 2.f);
		AddRow(renderer, "SCORE", player1.score, player2.score, firstRowY + rowSpacing * 4.f);
	}

	void CoopResultScene::InitializeSummary(SDL_Renderer* renderer)
	{
		const std::string total = "TEAM TOTAL " + std::to_string(GameData::CurrentScore);
		AddText(renderer, total, CentredX(total.size(), summaryFontSize), totalY, summaryFontSize, SDL_Color{ 255, 255, 0, 255 });

		const std::string mvp = MvpText();
		AddText(renderer, mvp, CentredX(mvp.size(), summaryFontSize), mvpY, summaryFontSize, SDL_Color{ 0, 255, 0, 255 });

		const std::string prompt = "PRESS ENTER TO CONTINUE";
		AddText(renderer, prompt, CentredX(prompt.size(), promptFontSize), promptY, promptFontSize, SDL_Color{ 255, 255, 0, 255 });
	}

	void CoopResultScene::InitializeInput()
	{
		InputHandler::Instance().BindFunction(SDLK_RETURN, [this]() { ReturnToMainMenu(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(0, XINPUT_GAMEPAD_A, [this]() { ReturnToMainMenu(); }, InputHandler::KeyAction::KeyUp);
	}

	void CoopResultScene::InitializeSounds()
	{
		auto& soundSystem = ServiceLocator::GetSoundSystem();
		soundSystem.AddSound("MenuTheme_1.mp3", SoundId::Music, true);
		soundSystem.PlaySound(SoundId::Music, soundSystem.GetCurrentVolume());
	}

	void CoopResultScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void CoopResultScene::ReturnToMainMenu()
	{
		SceneManager::Instance().QueueSceneChange("MainMenu");
	}
}
