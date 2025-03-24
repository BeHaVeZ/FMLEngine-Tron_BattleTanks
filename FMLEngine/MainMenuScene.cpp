#include "MainMenuScene.h"
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
#include "SelectMenuOptionCommand.h"
#include "MuteSoundCommand.h"

bool MainMenuScene::Initialize(SDL_Renderer* renderer)
{
	this->storedRenderer = renderer;
	selectedIndex = 0;

	InitializeBackground(renderer);
	InitializeMenuOptions(renderer);
	InitializeSelectionArrow(renderer);
	InitializeInput();
	InitializeSounds();

	return true;
}


void MainMenuScene::HandleInput(SDL_Event& event)
{
	InputHandler::Instance().HandleInput(event);
}

void MainMenuScene::InitializeInput() {
	InputHandler::Instance().BindCommand(SDLK_m, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);
	InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_Y, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);

	auto arrow = FindGameObjectByTag("SelectionArrow");
	if (arrow) {
		InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<RotateCommand>(arrow, 270.0f));
		InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<RotateCommand>(arrow, 90.0f));
		InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<RotateCommand>(arrow, 180.0f));
		InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<RotateCommand>(arrow, 0.0f));

		InputHandler::Instance().BindCommand(SDLK_e, std::make_unique<SelectMenuOptionCommand>(arrow));

		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_A, std::make_unique<SelectMenuOptionCommand>(arrow),InputHandler::KeyAction::KeyUp);
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<RotateCommand>(arrow, 270.0f));
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<RotateCommand>(arrow, 90.0f));
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<RotateCommand>(arrow, 180.0f));
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<RotateCommand>(arrow, 0.0f));
	}
}


void MainMenuScene::Update(float)
{
}

void MainMenuScene::Render(SDL_Renderer* renderer)
{
	for (auto& gameObject : gameObjects)
	{
		gameObject->Render(renderer);
	}

	for (auto& option : menuOptions) {
		option->Render(renderer);
	}
}

void MainMenuScene::Cleanup()
{
}

void MainMenuScene::InitializeBackground(SDL_Renderer* renderer)
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

void MainMenuScene::InitializeMenuOptions(SDL_Renderer* renderer)
{
	auto playOption = std::make_unique<GameObject>();
	auto playText = std::make_unique<TextComponent>("Play", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 0, 255, 255 }, renderer);
	playOption->GetComponent<TransformComponent>()->SetPosition({ 450, 300 });
	playOption->AddComponent(std::move(playText));

	menuOptions.push_back(playOption.get());
	gameObjects.push_back(std::move(playOption));

	auto quitOption = std::make_unique<GameObject>();
	auto quitText = std::make_unique<TextComponent>("Quit", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 0, 255, 255 }, renderer);
	quitOption->GetComponent<TransformComponent>()->SetPosition({ 450, 500 });
	quitOption->AddComponent(std::move(quitText));

	menuOptions.push_back(quitOption.get());
	gameObjects.push_back(std::move(quitOption));


	auto coopoption = std::make_unique<GameObject>();
	auto coopText = std::make_unique<TextComponent>("Coop", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 0, 255, 0, 255 }, renderer);
	coopoption->GetComponent<TransformComponent>()->SetPosition({ 270, 400 });
	coopoption->AddComponent(std::move(coopText));

	menuOptions.push_back(coopoption.get());
	gameObjects.push_back(std::move(coopoption));


	auto versusOption = std::make_unique<GameObject>();
	auto versusText = std::make_unique<TextComponent>("Versus", "data/fonts/tron-arcade.ttf", 32, SDL_Color{ 255, 0, 0, 255 }, renderer);
	versusOption->GetComponent<TransformComponent>()->SetPosition({ 630, 400 });
	versusOption->AddComponent(std::move(versusText));

	menuOptions.push_back(versusOption.get());
	gameObjects.push_back(std::move(versusOption));
}

void MainMenuScene::InitializeSelectionArrow(SDL_Renderer* renderer)
{
	selectionArrow = std::make_unique<GameObject>("SelectionArrow");

	auto arrowText = std::make_unique<TextComponent>("->", "data/fonts/Game_Of_Squids.ttf", 32, SDL_Color{ 255, 255, 255, 255 }, renderer);

	selectionArrow->GetComponent<TransformComponent>()->SetPosition({ 480, 400 });
	selectionArrow->GetComponent<TransformComponent>()->SetRotation(270);

	selectionArrow->AddComponent(std::move(arrowText));

	gameObjects.push_back(std::move(selectionArrow));
}

void MainMenuScene::InitializeSounds()
{
	ServiceLocator::GetSoundSystem().AddSound("Menu_Music.mp3", 1, true);
	ServiceLocator::GetSoundSystem().PlaySound(1, 0.f);
}
