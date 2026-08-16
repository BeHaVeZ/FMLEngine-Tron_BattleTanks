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
#include "../Tron_BattleTanks/ShootComponent.h"
#include "../Tron_BattleTanks/ShootCommand.h"
#include "../Tron_BattleTanks/SkipLevelCommand.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "SoundHelper.h"

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
			backgroundTransform->SetPosition({ ConfigManager::Instance().GetWindowWidth() / 2, ConfigManager::Instance().GetWindowHeight() / 2 });
			backgroundTransform->SetPivot({ 0.f, 0.f });
			backgroundTransform->SetSize(
				static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
				static_cast<float>(ConfigManager::Instance().GetWindowHeight())
			);
		}
		AddGameObject(std::move(background));

		float offset = 100.f;

		auto bg = FindGameObjectByTag("Background");
		bg->GetComponent<TransformComponent>()->SetSize((float)ConfigManager::Instance().GetWindowWidth(), (float)ConfigManager::Instance().GetWindowHeight() - offset);
		bg->GetComponent<TransformComponent>()->SetPosition({ 0,offset });

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


		GameObject* player = FindGameObjectByTag("Player1");

		auto healthUIPlayer1 = std::make_unique<GameObject>("HealthUIPlayer1");
		auto healthUIComponent = std::make_unique<HealthUIComponent>(player->GetComponent<HealthComponent>());
		healthUIPlayer1->AddComponent(std::move(healthUIComponent));
		healthUIPlayer1->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer1->GetComponent<TransformComponent>()->SetPosition({ 10, 30 });
		player->GetSubject().AddObserver(healthUIPlayer1->GetComponent<HealthUIComponent>());
		gameObjects.push_back(std::move(healthUIPlayer1));
	}

	void TestingScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateGreenTankPrefab({ 200,200 }, "Player1");
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
			wall->GetComponent<TransformComponent>()->SetPivot({ 0,0 });


			AddGameObject(std::move(wall));
		}
	}


	void TestingScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();
		InputBindingHelper::BindSoloModeControls(FindGameObjectByTag("Player1"));
	}

	void TestingScene::InitializeSounds()
	{
		SoundHelper::LoadSharedSounds();
	}


	void TestingScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
#ifdef _DEBUG
		HandleDebugMouseInput(event);
#endif
	}

#ifdef _DEBUG
	void TestingScene::HandleDebugMouseInput(const SDL_Event& event)
	{
		if (event.type != SDL_MOUSEBUTTONDOWN)
		{
			return;
		}

		const glm::vec2 clickPosition{ event.button.x, event.button.y };
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			if (isFirstClick)
			{
				firstClick = clickPosition;
				isFirstClick = false;
				Logger::Log(LogLevel::Info, "First click at: (%d, %d)", event.button.x, event.button.y);
				return;
			}

			const int width = event.button.x - static_cast<int>(firstClick.x);
			const int height = event.button.y - static_cast<int>(firstClick.y);
			isFirstClick = true;
			Logger::Log(LogLevel::Info, "Rectangle dimensions: {%d, %d, %d, %d},",
				static_cast<int>(firstClick.x), static_cast<int>(firstClick.y), width, height);
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			Logger::Log(LogLevel::Info, "Spawning recognizer at: (%d, %d)", event.button.x, event.button.y);
			AddGameObject(PrefabRegistry::Instance().CreateRecognizerPrefab(clickPosition));
		}
		else if (event.button.button == SDL_BUTTON_MIDDLE)
		{
			Logger::Log(LogLevel::Info, "Spawning tank at: (%d, %d)", event.button.x, event.button.y);
			AddGameObject(PrefabRegistry::Instance().CreateBlueTankPrefab(clickPosition));
		}
	}
#endif

	void TestingScene::Update(float deltaTime)
	{
		Scene::Update(deltaTime);
	}

	void TestingScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

}
