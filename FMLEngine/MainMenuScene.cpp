#include "MainMenuScene.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "GameStateManager.h"
#include "SceneManager.h"
#include <memory>
#include "ConfigManager.h"
#include "ServiceLocator.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "../Tron_BattleTanks/GameData.h"
#include "SoundHelper.h"

namespace FML
{

	bool MainMenuScene::Initialize(SDL_Renderer* renderer)
	{
		GameData::CurrentGameMode = GameData::GameMode::None;
		GameData::CurrentScore = 0;
		this->storedRenderer = renderer;

		InitializeBackground(renderer);
		InitializeMenuOptions(renderer);
		InitializeInput();
		InitializeSounds();


		GameData::ResetValues();
		selectedOption = menuOptions[0];
		UpdateMenuOptionHighlight();
		SelectPlay();
		return true;
	}


	void MainMenuScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void MainMenuScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();

		int controllerID = 0;

		InputHandler::Instance().BindFunction(SDLK_w, [this]() {SelectPlay(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_s, [this]() {SelectQuit(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_a, [this]() {SelectCoop(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_d, [this]() {SelectVersus(); },InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_RETURN, [this]() {ExecuteMenuOption(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_SPACE, [this]() {ExecuteMenuOption(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindFunction(SDLK_e, [this]() {ExecuteMenuOption(); }, InputHandler::KeyAction::KeyUp);

		InputHandler::Instance().BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_UP, [this]() {SelectPlay(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_DOWN, [this]() {SelectQuit(); }, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_LEFT, [this]() {SelectCoop();}, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_RIGHT, [this]() {SelectVersus();}, InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadFunction(controllerID, XINPUT_GAMEPAD_A, [this]() {ExecuteMenuOption();}, InputHandler::KeyAction::KeyUp);
	}


	void MainMenuScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void MainMenuScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void MainMenuScene::InitializeBackground(SDL_Renderer* renderer)
	{
		auto background = std::make_unique<GameObject>();
		auto backgroundTexture = std::make_unique<TextureComponent>("data/artassets/tron_bg.png", renderer);
		background->AddComponent(std::move(backgroundTexture));

		auto backgroundTransform = background->GetComponent<TransformComponent>();
		if (backgroundTransform) {
			backgroundTransform->SetPosition({ ConfigManager::Instance().GetWindowWidth() / 2, ConfigManager::Instance().GetWindowHeight() / 2 });
			backgroundTransform->SetSize(
				static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
				static_cast<float>(ConfigManager::Instance().GetWindowHeight())
			);
		}
		gameObjects.push_back(std::move(background));
	}

	void MainMenuScene::InitializeMenuOptions(SDL_Renderer* renderer)
	{
		auto playOption = std::make_unique<GameObject>("Play");
		auto playText = std::make_unique<TextComponent>("Play", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 0, 255, 255 }, renderer);
		playOption->GetComponent<TransformComponent>()->SetPosition({ 450, 300 });
		playOption->AddComponent(std::move(playText));

		menuOptions.push_back(playOption.get());
		gameObjects.push_back(std::move(playOption));

		auto quitOption = std::make_unique<GameObject>("Quit");
		auto quitText = std::make_unique<TextComponent>("Quit", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 0, 255, 255 }, renderer);
		quitOption->GetComponent<TransformComponent>()->SetPosition({ 450, 500 });
		quitOption->AddComponent(std::move(quitText));

		menuOptions.push_back(quitOption.get());
		gameObjects.push_back(std::move(quitOption));


		auto coopoption = std::make_unique<GameObject>("Coop");
		auto coopText = std::make_unique<TextComponent>("Coop", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 255, 0, 255 }, renderer);
		coopoption->GetComponent<TransformComponent>()->SetPosition({ 270, 400 });
		coopoption->AddComponent(std::move(coopText));

		menuOptions.push_back(coopoption.get());
		gameObjects.push_back(std::move(coopoption));


		auto versusOption = std::make_unique<GameObject>("Versus");
		auto versusText = std::make_unique<TextComponent>("Versus", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 255, 0, 0, 255 }, renderer);
		versusOption->GetComponent<TransformComponent>()->SetPosition({ 630, 400 });
		versusOption->AddComponent(std::move(versusText));

		menuOptions.push_back(versusOption.get());
		gameObjects.push_back(std::move(versusOption));

		UpdateMenuOptionHighlight();
	}

	void MainMenuScene::UpdateMenuOptionHighlight()
	{
		for (auto* option : menuOptions)
		{
			SDL_Color baseColor = { 0, 255, 255, 255 };

			if (option->GetTag() == "Coop")
				baseColor = { 0, 255, 0, 255 };
			else if (option->GetTag() == "Versus")
				baseColor = { 255, 0, 0, 255 };

			SDL_Color finalColor = baseColor;
			finalColor.a = (option == selectedOption) ? 255 : 100;

			auto* text = option->GetComponent<TextComponent>();
			if (!text) continue;

			text->SetColor(finalColor, SceneManager::Instance().GetRenderer());
		}
	}

	void MainMenuScene::SelectPlay()
	{
		if (selectedOption->GetTag() == "Play") return;
		selectedOption = FindGameObjectByTag("Play");
		SoundHelper::PlayRandomSound({ 15,16,17 });
		UpdateMenuOptionHighlight();
	}

	void MainMenuScene::SelectQuit()
	{
		if (selectedOption->GetTag() == "Quit") return;
		selectedOption = FindGameObjectByTag("Quit");
		SoundHelper::PlayRandomSound({ 15,16,17 });
		UpdateMenuOptionHighlight();
	}

	void MainMenuScene::SelectCoop()
	{
		if (selectedOption->GetTag() == "Coop") return;
		selectedOption = FindGameObjectByTag("Coop");
		SoundHelper::PlayRandomSound({ 15,16,17 });
		UpdateMenuOptionHighlight();
	}

	void MainMenuScene::SelectVersus()
	{
		if (selectedOption->GetTag() == "Versus") return;
		selectedOption = FindGameObjectByTag("Versus");
		SoundHelper::PlayRandomSound({ 15,16,17 });
		UpdateMenuOptionHighlight();
	}

	void MainMenuScene::ExecuteMenuOption()
	{
		if (!selectedOption) return;

		std::string tag = selectedOption->GetTag();

		if (tag == "Play") 
		{
			GameData::CurrentGameMode = GameData::GameMode::Solo;
			SceneManager::Instance().QueueSceneChange("Solo");
		}
		if (tag == "Coop") 
		{
			GameData::CurrentGameMode = GameData::GameMode::Coop;
			SceneManager::Instance().QueueSceneChange("Coop");
		}
		if (tag == "Versus") 
		{
			GameData::CurrentGameMode = GameData::GameMode::Versus;
			SceneManager::Instance().QueueSceneChange("Versus");
		}
		if (tag == "Quit") 
		{
			GameStateManager::Instance().SetRunning(false);
		}
	}


	void MainMenuScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("blunk_1.wav", 15, false);
		ServiceLocator::GetSoundSystem().AddSound("blunk_2.wav", 16, false);
		ServiceLocator::GetSoundSystem().AddSound("blunk_3.wav", 17, false);
		ServiceLocator::GetSoundSystem().SetVolume(0.5f);
		ServiceLocator::GetSoundSystem().AddSound("MenuTheme_1.mp3", 1, true);
		ServiceLocator::GetSoundSystem().PlaySound(1, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}
}

