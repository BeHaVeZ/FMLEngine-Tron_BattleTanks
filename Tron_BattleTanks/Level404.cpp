#include "PrefabRegistry.h"
#include "Level404.h"
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
#include "SkipLevelCommand.h"
#include "InputBindingHelper.h"
#include "EnemyManagerComponent.h"


namespace FML
{

	const std::string backgroundImagePath = "data/levels/level404.png";

	bool Level404::Initialize(SDL_Renderer* renderer)
	{
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();

		InitializeUI();
		InitializeWalls();
		InitializeManagers();
		InitializeCenterTP();


		InitializeInput();
		InitializeSounds();

		return true;
	}

	void Level404::InitializeBackground(SDL_Renderer* renderer) {
		auto background = std::make_unique<GameObject>("Background");
		auto backgroundTexture = std::make_unique<TextureComponent>(backgroundImagePath, renderer);
		background->AddComponent(std::move(backgroundTexture));

		auto backgroundTransform = background->GetComponent<TransformComponent>();
		if (backgroundTransform) {
			backgroundTransform->SetPosition({ ConfigManager::Instance().GetWindowWidth() / 2, ConfigManager::Instance().GetWindowHeight() / 2 });
			backgroundTransform->SetPivot({ 0.f, 0.f });
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

	void Level404::InitializeTitle(SDL_Renderer* renderer)
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

	void Level404::InitializeFPSCounter(SDL_Renderer* renderer)
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

	void Level404::InitializeUI()
	{
		InitializeHealthUI();
		InitializeScoreUI();
	}

	void Level404::InitializeHealthUI()
	{
		auto healthUIPlayer1 = PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10,30 }, "HealthUIPlayer1");
		gameObjects.push_back(std::move(healthUIPlayer1));
	}

	void Level404::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 514,428 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void Level404::InitializeScoreUI()
	{
		auto highScoreUI = PrefabRegistry::Instance().CreateHighScoreUI({ 400,30 }, "HighScoreUI");
		gameObjects.push_back(std::move(highScoreUI));

		auto currentScoreUI = PrefabRegistry::Instance().CreateCurrentScoreUI({ 750,30 }, "CurrentScoreUI");
		gameObjects.push_back(std::move(currentScoreUI));
	}


	void Level404::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();

		auto tank = FindGameObjectByTag("Player1");
		if (tank)
		{
			InputBindingHelper::BindSoloModeControls(tank);
		}
	}

	void Level404::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("SoloTheme_404.mp3", 1, true);

		SoundHelper::LoadSharedSounds();

		ServiceLocator::GetSoundSystem().PlaySound(1, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}

	void Level404::InitializeManagers()
	{
		auto enemyManager = PrefabRegistry::Instance().CreateEnemyManager();
		auto enemyManagement = enemyManager->GetComponent<EnemyManagerComponent>();
		enemyManagement->SetMaxBlueTanks(0);
		enemyManagement->SetMaxPinkTanks(0);
		enemyManagement->SetMaxRecognizers(10);
		enemyManagement->SetSpawnCooldown(.5f);


		AddGameObject(std::move(enemyManager));
	}

	void Level404::InitializeCenterTP()
	{
		auto centerTP = PrefabRegistry::Instance().CreateTeleportCenterPrefab();
		AddGameObject(std::move(centerTP));
	}

	void Level404::InitializeWalls()
	{
		FileReader reader("data/levels/level02C.txt");
		auto walls = reader.ReadRectangles();

		for (const auto& rect : walls) {
			auto wall = std::make_unique<GameObject>("Wall");

			SDL_Rect box = { rect.x, rect.y, rect.w, rect.h };

			auto wallCollider = std::make_unique<BoxCollider>(box);
			wallCollider->isStatic = true;
			wall->AddComponent(std::move(wallCollider));

			wall->GetComponent<TransformComponent>()->SetPosition({ rect.x, rect.y });
			wall->GetComponent<TransformComponent>()->SetPivot({ 0,0 });


			AddGameObject(std::move(wall));
		}
	}


	void Level404::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void Level404::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void Level404::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void Level404::OnExit()
	{
		GameAdmin::Instance().ResetPlayers();
	}
}