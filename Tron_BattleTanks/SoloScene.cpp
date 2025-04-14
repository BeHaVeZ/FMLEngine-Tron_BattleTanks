#include "PrefabRegistry.h"
#include "SoloScene.h"
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
#include "FileReader.h"
#include "BoxCollider.h"
#include "TestCommand.h"
#include "ShootCommand.h"

namespace FML
{

	const std::string backgroundImagePath = "data/levels/level00.png";

	bool SoloScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();

		InitializeUI();
		InitializeWalls();

		InitializeInput();
		InitializeSounds();

		return true;
	}

	void SoloScene::InitializeBackground(SDL_Renderer* renderer) {
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
		bg->GetComponent<TransformComponent>()->SetPosition({ 0,offset });

	}

	void SoloScene::InitializeTitle(SDL_Renderer* renderer)
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

	void SoloScene::InitializeFPSCounter(SDL_Renderer* renderer)
	{
		auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

		SDL_Color fpsColor = { 255, 255, 255, 255 };

		auto titleTextComponent = std::make_unique<TextComponent>("FPS 0", "data/fonts/tron-arcade.ttf", 10, fpsColor, renderer);
		fpsGameObject->AddComponent(std::move(titleTextComponent));

		auto fpsComponent = std::make_unique<FPSComponent>(renderer);
		fpsGameObject->AddComponent(std::move(fpsComponent));
		fpsGameObject->GetComponent<FPSComponent>()->Initialize();
		fpsGameObject->GetComponent<TransformComponent>()->SetPosition({ 10, 10 });

		gameObjects.push_back(std::move(fpsGameObject));
	}

	void SoloScene::InitializeUI()
	{
		InitializeHealthUI();
		InitializeScoreUI();
	}

	void SoloScene::InitializeHealthUI()
	{
		auto healthUIPlayer1 = PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10,30 }, 3, "HealthUIPlayer1");
		gameObjects.push_back(std::move(healthUIPlayer1));
		//auto healthUIPlayer2 = PrefabRegistry::Instance().CreateHealthUIForPlayer2({ 200,30 }, 3, "HealthUIPlayer2");
		//gameObjects.push_back(std::move(healthUIPlayer2));
	}

	void SoloScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 200,200 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void SoloScene::InitializeScoreUI()
	{
		auto highScoreUI = PrefabRegistry::Instance().CreateHighScoreUI({ 400,30 }, "HighScoreUI");
		gameObjects.push_back(std::move(highScoreUI));

		auto currentScoreUI = PrefabRegistry::Instance().CreateCurrentScoreUI({ 750,30 }, "CurrentScoreUI");
		gameObjects.push_back(std::move(currentScoreUI));
	}


	void SoloScene::InitializeInput()
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

			InputHandler::Instance().BindCommand(SDLK_r, std::make_unique<DamageCommand>(tank1, 1), InputHandler::KeyAction::KeyUp);
			InputHandler::Instance().BindCommand(SDLK_SPACE, std::make_unique<ShootCommand>(tank1->FindChildByTag("Turret")), InputHandler::KeyAction::KeyUp);

			int controllerId = 0;
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 100.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_RIGHT_SHOULDER, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), 1.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_LEFT_SHOULDER, std::make_unique<RotateTurretCommand>(tank1->FindChildByTag("Turret"), -1.f));
			InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_A, std::make_unique<ShootCommand>(tank1->FindChildByTag("Turret")), InputHandler::KeyAction::KeyUp);
		}
		InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_Y, std::make_unique<MuteSoundCommand>(), InputHandler::KeyAction::KeyUp);
	}

	void SoloScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("AyoWhatV3.wav", 1, true);
		ServiceLocator::GetSoundSystem().PlaySound(1, .0f);
	}

	void SoloScene::InitializeWalls()
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


	void SoloScene::HandleInput(SDL_Event& event) {
		InputHandler::Instance().HandleInput(event);
	}

	void SoloScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void SoloScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void SoloScene::Cleanup()
	{
	}
}