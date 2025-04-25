#include "PrefabRegistry.h"
#include "SoloLevel3.h"
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


namespace FML
{

	const std::string backgroundImagePath = "data/levels/level02.png";

	bool SoloLevel3::Initialize(SDL_Renderer* renderer)
	{
		Logger::Log(LogLevel::Error, "Score at start of SoloScene3: %d", GameData::CurrentScore);
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();

		InitializeUI();
		InitializeWalls();
		InitializeManagers();


		InitializeInput();
		InitializeSounds();

		return true;
	}

	void SoloLevel3::InitializeBackground(SDL_Renderer* renderer) {
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

	void SoloLevel3::InitializeTitle(SDL_Renderer* renderer)
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

	void SoloLevel3::InitializeFPSCounter(SDL_Renderer* renderer)
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

	void SoloLevel3::InitializeUI()
	{
		InitializeHealthUI();
		InitializeScoreUI();
	}

	void SoloLevel3::InitializeHealthUI()
	{
		auto healthUIPlayer1 = PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10,30 }, 3, "HealthUIPlayer1");
		gameObjects.push_back(std::move(healthUIPlayer1));
	}

	void SoloLevel3::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 57,118 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void SoloLevel3::InitializeScoreUI()
	{
		auto highScoreUI = PrefabRegistry::Instance().CreateHighScoreUI({ 400,30 }, "HighScoreUI");
		gameObjects.push_back(std::move(highScoreUI));

		auto currentScoreUI = PrefabRegistry::Instance().CreateCurrentScoreUI({ 750,30 }, "CurrentScoreUI");
		gameObjects.push_back(std::move(currentScoreUI));
	}


	void SoloLevel3::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();

		auto tank = FindGameObjectByTag("Player1");
		if (tank)
		{
			InputBindingHelper::BindSoloModeControls(tank);
		}
	}

	void SoloLevel3::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("AyoWhatV2.wav", 1, true);

		SoundHelper::LoadSharedSounds();

		ServiceLocator::GetSoundSystem().PlaySound(1, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}

	void SoloLevel3::InitializeManagers()
	{
		auto enemyManager = PrefabRegistry::Instance().CreateEnemyManager();
		AddGameObject(std::move(enemyManager));
	}

	void SoloLevel3::InitializeWalls()
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

			AddGameObject(std::move(wall));
		}
	}


	void SoloLevel3::HandleInput(SDL_Event& event) 
	{
		InputHandler::Instance().HandleInput(event);
	}

	void SoloLevel3::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void SoloLevel3::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void SoloLevel3::OnExit()
	{
		GameAdmin::Instance().ResetPlayers();
	}
}