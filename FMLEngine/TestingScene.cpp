#include "PrefabRegistry.h"
#include "TestingScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "ServiceLocator.h"
#include <iostream>
#include "MuteSoundCommand.h"
#include "MoveCommand.h"
#include "RotateTurretCommand.h"
#include "DamageCommand.h"
#include "HealthUIComponent.h"
#include "DebugDraw.h"
#include "TestCommand.h"
#include "BoxCollider.h"
#include "../Tron_BattleTanks/FileReader.h"

namespace FML
{

	const std::string backgroundImagePath = "data/levels/level00.png";

	bool TestingScene::Initialize(SDL_Renderer* renderer) {
		InitializeBackground(renderer);

		InitializeFirstTank();
		InitializeFPSCounter(renderer);

		InitializeWalls();

		InitializeInput();
		InitializeSounds();

		return true;
	}

	void TestingScene::InitializeBackground(SDL_Renderer* renderer) {
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

		float offset = 100.f;

		auto bg = FindGameObjectByTag("Background");
		bg->GetComponent<TransformComponent>()->SetSize((float)ConfigManager::Instance().GetWindowWidth(), (float)ConfigManager::Instance().GetWindowHeight() - offset);
		bg->GetComponent<TransformComponent>()->SetPosition({0,offset});
	}

	void TestingScene::InitializeFPSCounter(SDL_Renderer* renderer)
	{
		auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

		SDL_Color fpsColor = { 255, 255, 255, 255 };

		auto titleTextComponent = std::make_unique<TextComponent>(
			"FPS 0",
			"data/fonts/tron-arcade.ttf",
			10,
			fpsColor,
			renderer);
		fpsGameObject->AddComponent(std::move(titleTextComponent));


		auto fpsComponent = std::make_unique<FPSComponent>(renderer);
		fpsGameObject->AddComponent(std::move(fpsComponent));
		fpsGameObject->GetComponent<FPSComponent>()->Initialize();

		fpsGameObject->GetComponent<TransformComponent>()->SetPosition({ 10, 10 });

		gameObjects.push_back(std::move(fpsGameObject));


		auto healthUIPlayer1 = std::make_unique<GameObject>("HealthUIPlayer1");
		auto healthUIComponent = std::make_unique<HealthUIComponent>(100);
		healthUIPlayer1->AddComponent(std::move(healthUIComponent));
		healthUIPlayer1->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer1->GetComponent<TransformComponent>()->SetPosition({ 10, 30 });
		FindGameObjectByTag("Player1")->GetSubject().AddObserver(healthUIPlayer1->GetComponent<HealthUIComponent>());
		gameObjects.push_back(std::move(healthUIPlayer1));
	}

	void TestingScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 200,200 }, "Player1");
		AddGameObject(std::move(tank));

		tank = PrefabRegistry::Instance().CreateBlueTankPrefab({ 300,200 }, "Player2");

		AddGameObject(std::move(tank));
	}

	void TestingScene::InitializeWalls()
	{
		FileReader reader("data/levels/level00C.txt");
		auto walls = reader.ReadRectangles();

		for (const auto& rect : walls) {
			auto wall = std::make_unique<GameObject>("Wall");

			SDL_Rect box = { rect.x, rect.y, rect.w, rect.h };

			auto wallCollider = std::make_unique<BoxCollider>(box);
			wallCollider->isStatic = true;
			wall->AddComponent(std::move(wallCollider));

			wall->GetComponent<TransformComponent>()->SetPosition({ rect.x, rect.y });

			AddGameObject(std::move(wall));
		}
	}


	void TestingScene::InitializeInput()
	{
		InputHandler::Instance().BindCommand(SDLK_p, std::make_unique<TestCommand>(), InputHandler::KeyAction::KeyUp);


		auto tank1 = FindGameObjectByTag("Player1");
		if (tank1)
		{
			InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 100.f));

			InputHandler::Instance().BindCommand(SDLK_e, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindCommand(SDLK_q, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), -1.f));

			InputHandler::Instance().BindCommand(SDLK_r, std::make_unique<DamageCommand>(tank1, 10), InputHandler::KeyAction::KeyUp);

			int controllerId = 0;
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 200.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_RIGHT_SHOULDER, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_LEFT_SHOULDER,std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), -1.f));
		}
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_Y, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);
	}

	void TestingScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("AyoWhatV3.wav", 1, true);
		ServiceLocator::GetSoundSystem().PlaySound(1, .0f);
	}


	void TestingScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void TestingScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void TestingScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void TestingScene::Cleanup()
	{
	}
}


