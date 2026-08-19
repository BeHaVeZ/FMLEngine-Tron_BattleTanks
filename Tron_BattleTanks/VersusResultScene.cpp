#include "VersusResultScene.h"
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
		float CentredX(size_t characters, int fontSize)
		{
			const float glyphWidth = fontSize * 0.875f;
			const float textWidth = characters * glyphWidth;
			return (ConfigManager::Instance().GetWindowWidth() - textWidth) * 0.5f;
		}

		std::string ResultText()
		{
			switch (GameData::VersusWinner)
			{
			case 1:  return "PLAYER 1 WINS";
			case 2:  return "PLAYER 2 WINS";
			default: return "DRAW";
			}
		}
	}

	bool VersusResultScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeResultText(renderer);
		InitializeInput();
		InitializeSounds();

		return true;
	}

	void VersusResultScene::InitializeResultText(SDL_Renderer* renderer)
	{
		const std::string result = ResultText();

		auto titleObject = std::make_unique<GameObject>();
		titleObject->AddComponent(std::make_unique<TextComponent>(
			result, "data/fonts/tron-arcade.ttf", titleFontSize, SDL_Color{ 0, 255, 0, 255 }, renderer));
		titleObject->GetComponent<TransformComponent>()->SetPosition(
			{ CentredX(result.size(), titleFontSize), titleY });
		AddGameObject(std::move(titleObject));

		const std::string prompt = "PRESS ENTER TO CONTINUE";

		auto promptObject = std::make_unique<GameObject>();
		promptObject->AddComponent(std::make_unique<TextComponent>(
			prompt, "data/fonts/tron-arcade.ttf", promptFontSize, SDL_Color{ 255, 255, 0, 255 }, renderer));
		promptObject->GetComponent<TransformComponent>()->SetPosition(
			{ CentredX(prompt.size(), promptFontSize), promptY });
		AddGameObject(std::move(promptObject));
	}

	void VersusResultScene::InitializeInput()
	{
		InputHandler::Instance().BindFunction(SDLK_RETURN, [this]() { ReturnToMainMenu(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(0, XINPUT_GAMEPAD_A, [this]() { ReturnToMainMenu(); }, InputHandler::KeyAction::KeyUp);
	}

	void VersusResultScene::InitializeSounds()
	{
		auto& soundSystem = ServiceLocator::GetSoundSystem();
		soundSystem.AddSound("MenuTheme_1.mp3", SoundId::Music, true);
		soundSystem.PlaySound(SoundId::Music, soundSystem.GetCurrentVolume());
	}

	void VersusResultScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void VersusResultScene::ReturnToMainMenu()
	{
		SceneManager::Instance().QueueSceneChange("MainMenu");
	}
}
