#include "SoloHighscoreScene.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "GameStateManager.h"
#include "SceneManager.h"
#include <memory>
#include <iostream>
#include "ConfigManager.h"
#include "ServiceLocator.h"
#include "RotateCommand.h"
#include "MuteSoundCommand.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "HighscoreManager.h"

namespace FML
{

	bool SoloHighscoreScene::Initialize(SDL_Renderer* renderer)
	{
		this->storedRenderer = renderer;

		HighscoreManager manager("data/highscores.txt");
		auto scores = manager.GetTopScores(10);

		auto titleGameObject = std::make_unique<GameObject>();

		auto title = std::make_unique<TextComponent>("HIGHSCORES", "data/fonts/tron-arcade.ttf", 24, SDL_Color{0,255,0,255}, renderer);
		titleGameObject->GetComponent<TransformComponent>()->SetPosition({ 405, 100 });
		titleGameObject->AddComponent(std::move(title));

		AddGameObject(std::move(titleGameObject));

		float startY = 150.f;
		float spacing = 40.f;

		for (size_t i = 0; i < scores.size(); ++i)
		{
			const auto& entry = scores[i];
			auto go = std::make_unique<GameObject>();

			std::stringstream ss;
			ss << entry.name << " " << entry.score;

			auto text = std::make_unique<TextComponent>(ss.str(), "data/fonts/tron-arcade.ttf", 24,SDL_Color{0,255,0,255}, renderer);
			go->AddComponent(std::move(text));
			go->GetComponent<TransformComponent>()->SetPosition({ 430.f, startY + i * spacing });

			AddGameObject(std::move(go));
		}
		InitializeInput();
		InitializeSounds();

		return true;
	}


	void SoloHighscoreScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void SoloHighscoreScene::InitializeInput()
	{
		//InputBindingHelper::BindGlobalCommands();
		InputHandler::Instance().BindFunction(SDLK_RETURN, [this]() { RestartGame(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(0, XINPUT_GAMEPAD_A, [this]() { RestartGame(); }, InputHandler::KeyAction::KeyUp);
	}


	void SoloHighscoreScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void SoloHighscoreScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void SoloHighscoreScene::InitializeBackground(SDL_Renderer* renderer)
	{
		auto background = std::make_unique<GameObject>();
		auto backgroundTexture = std::make_unique<TextureComponent>("data/artassets/tron_bg.png", renderer);
		background->AddComponent(std::move(backgroundTexture));

		auto backgroundTransform = background->GetComponent<TransformComponent>();
		if (backgroundTransform) {
			backgroundTransform->SetPosition({ 0,0 });
			backgroundTransform->SetSize(
				static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
				static_cast<float>(ConfigManager::Instance().GetWindowHeight())
			);
		}
		gameObjects.push_back(std::move(background));
	}

	void SoloHighscoreScene::RestartGame()
	{
		SceneManager::Instance().QueueSceneChange("MainMenu");
	}

	void SoloHighscoreScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("Menu_Music.mp3", SoundId::Music, true);
		ServiceLocator::GetSoundSystem().PlaySound(SoundId::Music, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}
}

