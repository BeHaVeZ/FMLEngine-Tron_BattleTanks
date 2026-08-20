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
#include "RotateTurretCommand.h"
#include "BoxCollider.h"
#include "RotateCommand.h"
#include "DamageCommand.h"
#include "MuteSoundCommand.h"
#include "../Tron_BattleTanks/FileReader.h"
#include "../Tron_BattleTanks/ShootCommand.h"
#include "../Tron_BattleTanks/SkipLevelCommand.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "../Tron_BattleTanks/GameData.h"
#include <TestCommand.h>

namespace FML
{

	const std::string backgroundImagePath = "data/levels/level00.png";

	bool VersusScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);

		InitializeFirstTank();
		InitializeSecondTank();

		InitializeUI();

		InitializeWalls();
		InitializeCenterTP();

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
			backgroundTransform->SetPosition({ ConfigManager::Instance().GetWindowWidth() , ConfigManager::Instance().GetWindowHeight() });
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

	void VersusScene::InitializeFPSCounter(SDL_Renderer* renderer)
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

	void VersusScene::InitializeFirstTank()
	{
		auto tank = PrefabRegistry::Instance().CreateRedTankPrefab({ 57,118 }, "Player1");
		gameObjects.push_back(std::move(tank));
	}

	void VersusScene::InitializeSecondTank()
	{
		auto tank = PrefabRegistry::Instance().CreateYellowTankPrefab({ 938,705 }, "Player2");
		gameObjects.push_back(std::move(tank));
	}

	void VersusScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();
		InputBindingHelper::BindPauseControls();

		auto tankP1 = FindGameObjectByTag("Player1");
		auto tankP2 = FindGameObjectByTag("Player2");
		if (tankP1 && tankP2)
		{
			InputBindingHelper::BindDuoModeControls(tankP1, tankP2);
		}
	}

	void VersusScene::InitializeUI()
	{
		InitializeHealthUI();
		InitializeScoreUI();
	}

	void VersusScene::InitializeHealthUI()
	{
		auto healthUIPlayer1 = PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10,30 }, "HealthUIPlayer1");
		gameObjects.push_back(std::move(healthUIPlayer1));
		auto healthUIPlayer2 = PrefabRegistry::Instance().CreateHealthUIForPlayer2({ 200,30 }, "HealthUIPlayer2");
		gameObjects.push_back(std::move(healthUIPlayer2));
	}

	void VersusScene::InitializeScoreUI()
	{
		//VERSUS TODO WHAT SHOULD THIS BE?
	}

	void VersusScene::InitializeCenterTP()
	{
		auto centerTP = PrefabRegistry::Instance().CreateTeleportCenterPrefab();
		AddGameObject(std::move(centerTP));
	}

	void VersusScene::InitializeWalls()
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
			wall->GetComponent<TransformComponent>()->SetPivot({0,0});
			

			AddGameObject(std::move(wall));
		}
	}

	void VersusScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("VersusTheme_1.wav", SoundId::Music, true);

		SoundHelper::LoadSharedSounds();

		ServiceLocator::GetSoundSystem().PlaySound(SoundId::Music, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}


	void VersusScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void VersusScene::Update(float deltaTime) 
	{
		Scene::Update(deltaTime);
	}

	void VersusScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}
	void VersusScene::OnExit()
	{
		GameAdmin::Instance().ResetPlayers();
	}
}

