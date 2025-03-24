#include "VersusScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "ServiceLocator.h"
#include "MoveCommand.h"
#include <iostream>
#include "PrefabRegistry.h"

namespace FML
{

	const std::string backgroundImagePath = "data/levels/level00.png";

	bool VersusScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();
		InitializeSecondTank();

		InitializeInput();
		InitializeSounds();

		return true;
	}

	void VersusScene::InitializeBackground(SDL_Renderer* renderer)
	{
		auto background = std::make_unique<GameObject>("Background");
		auto backgroundTexture = std::make_unique<TextureComponent>(backgroundImagePath, renderer);
		background->AddComponent(std::move(backgroundTexture));

		auto backgroundTransform = background->GetComponent<TransformComponent>();
		if (backgroundTransform) {
			backgroundTransform->SetPosition({ 0, 0 });
			backgroundTransform->SetSize(
				static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
				static_cast<float>(ConfigManager::Instance().GetWindowHeight())
			);
		}
		gameObjects.push_back(std::move(background));
	}

	void VersusScene::InitializeTitle(SDL_Renderer* renderer)
	{
		auto title = std::make_unique<GameObject>("title");

		SDL_Color color = { 0, 255, 0, 255 };

		auto titleTextComponent = std::make_unique<TextComponent>(
			"TRON Battle Tanks",
			"data/fonts/tron-arcade.ttf",
			24,
			color,
			renderer);

		title->AddComponent(std::move(titleTextComponent));
		title->GetComponent<TransformComponent>()->SetPosition({ 300, 300 });

		gameObjects.push_back(std::move(title));
	}

	void VersusScene::InitializeFPSCounter(SDL_Renderer* renderer)
	{
		auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

		SDL_Color fpsColor = { 255, 255, 255, 255 };

		auto titleTextComponent = std::make_unique<TextComponent>(
			"FPS 0",
			"data/fonts/tron-arcade.ttf",
			22,
			fpsColor,
			renderer);
		fpsGameObject->AddComponent(std::move(titleTextComponent));


		auto fpsComponent = std::make_unique<FPSComponent>(renderer);
		fpsGameObject->AddComponent(std::move(fpsComponent));
		fpsGameObject->GetComponent<FPSComponent>()->Initialize();

		fpsGameObject->GetComponent<TransformComponent>()->SetPosition({ 10, 10 });

		gameObjects.push_back(std::move(fpsGameObject));
	}

	void VersusScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 500,500 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void VersusScene::InitializeSecondTank()
	{
		auto tank = PrefabRegistry::Instance().CreateYellowTankPrefab({ 100,100 }, "Player2");
		gameObjects.push_back(std::move(tank));
	}

	void VersusScene::InitializeInput()
	{
		auto tank = FindGameObjectByTag("Player1");
		if (tank)
		{
			InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<MoveCommand>(tank, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<MoveCommand>(tank, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<MoveCommand>(tank, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<MoveCommand>(tank, glm::vec2(1, 0), 100.f));
		}

		tank = FindGameObjectByTag("Player2");
		if (tank)
		{
			int controllerId = 0;
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank, glm::vec2(0, -1), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank, glm::vec2(0, 1), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank, glm::vec2(-1, 0), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank, glm::vec2(1, 0), 200.f));
			InputHandler::Instance().BindCommand(SDLK_UP, std::make_unique<MoveCommand>(tank, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_DOWN, std::make_unique<MoveCommand>(tank, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_LEFT, std::make_unique<MoveCommand>(tank, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindCommand(SDLK_RIGHT, std::make_unique<MoveCommand>(tank, glm::vec2(1, 0), 100.f));
		}
	}

	void VersusScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("AyoWhat.wav", 1, true);
		ServiceLocator::GetSoundSystem().PlaySound(1, .5f);
	}


	void VersusScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void VersusScene::Update(float deltaTime) {
		for (auto& gameObject : gameObjects) {
			gameObject->Update(deltaTime);
		}
	}

	void VersusScene::Render(SDL_Renderer* renderer)
	{

		for (auto& gameObject : gameObjects) {
			gameObject->Render(renderer);
		}
	}

	void VersusScene::Cleanup()
	{
	}

}

