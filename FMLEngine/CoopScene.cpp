#include "CoopScene.h"
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
#include "BoxCollider.h"
#include "RotateCommand.h"
#include "RotateTurretCommand.h"
#include "DamageCommand.h"
#include "MuteSoundCommand.h"
#include "../Tron_BattleTanks/FileReader.h"
#include "../Tron_BattleTanks/ShootCommand.h"
#include "../Tron_BattleTanks/SkipLevelCommand.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "TestCommand.h"

namespace FML
{

	const std::string backgroundImagePath = "data/levels/level00.png";

	bool CoopScene::Initialize(SDL_Renderer* renderer) {
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();
		InitializeSecondTank();

		InitializeUI();

		InitializeWalls();

		InitializeInput();
		InitializeSounds();

		return true;
	}

	void CoopScene::InitializeBackground(SDL_Renderer* renderer) {
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

	void CoopScene::InitializeTitle(SDL_Renderer* renderer)
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

	void CoopScene::InitializeFPSCounter(SDL_Renderer* renderer)
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

	void CoopScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 360,335 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void CoopScene::InitializeSecondTank()
	{
		auto tank = PrefabRegistry::Instance().CreateYellowTankPrefab({ 640,485 }, "Player2");
		gameObjects.push_back(std::move(tank));
	}

	void CoopScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();

		auto tankP1 = FindGameObjectByTag("Player1");
		auto tankP2 = FindGameObjectByTag("Player2");
		if (tankP1 && tankP2)
		{
			InputBindingHelper::BindDuoModeControls(tankP1, tankP2);
		}
	}

	void CoopScene::InitializeUI()
	{
		InitializeHealthUI();
		InitializeScoreUI();
	}

	void CoopScene::InitializeHealthUI()
	{
		auto healthUIPlayer1 = PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10,30 }, 3, "HealthUIPlayer1");
		gameObjects.push_back(std::move(healthUIPlayer1));
		auto healthUIPlayer2 = PrefabRegistry::Instance().CreateHealthUIForPlayer2({ 200,30 }, 3, "HealthUIPlayer2");
		gameObjects.push_back(std::move(healthUIPlayer2));
	}

	void CoopScene::InitializeScoreUI()
	{
		auto highScoreUI = PrefabRegistry::Instance().CreateHighScoreUI({ 400,30 }, "HighScoreUI");
		gameObjects.push_back(std::move(highScoreUI));

		auto currentScoreUI = PrefabRegistry::Instance().CreateCurrentScoreUI({ 750,30 }, "CurrentScoreUI");
		gameObjects.push_back(std::move(currentScoreUI));
	}

	void CoopScene::InitializeWalls()
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

	void CoopScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("Menu_Music.mp3", 1, true);
		ServiceLocator::GetSoundSystem().PlaySound(1, 1.f);
	}


	void CoopScene::HandleInput(SDL_Event& event) {
		InputHandler::Instance().HandleInput(event);
	}

	void CoopScene::Update(float deltaTime) 
	{
		Scene::Update(deltaTime);
	}

	void CoopScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}
}

