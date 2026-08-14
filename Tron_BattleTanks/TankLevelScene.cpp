#include "TankLevelScene.h"
#include "BoxCollider.h"
#include "ConfigManager.h"
#include "EnemyManagerComponent.h"
#include "FPSComponent.h"
#include "FileReader.h"
#include "GameAdmin.h"
#include "GameObject.h"
#include "InputBindingHelper.h"
#include "InputHandler.h"
#include "PrefabRegistry.h"
#include "ServiceLocator.h"
#include "SoundHelper.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include <utility>

namespace FML
{
	TankLevelScene::TankLevelScene(std::string sceneName, LevelConfig levelConfig)
		: Scene(sceneName), config(std::move(levelConfig))
	{
	}

	bool TankLevelScene::Initialize(SDL_Renderer* renderer)
	{
		InitializeBackground(renderer);
		InitializeFPSCounter(renderer);
		InitializePlayer();
		InitializeUI();
		InitializeWalls();
		InitializeManagers();
		AddGameObject(PrefabRegistry::Instance().CreateTeleportCenterPrefab());
		InitializeInput();
		InitializeSounds();
		return true;
	}

	void TankLevelScene::InitializeBackground(SDL_Renderer* renderer)
	{
		auto background = std::make_unique<GameObject>("Background");
		background->AddComponent(std::make_unique<TextureComponent>(config.backgroundPath, renderer));
		auto* transform = background->GetComponent<TransformComponent>();
		transform->SetPivot({ 0.f, 0.f });
		transform->SetPosition({ 0.f, 100.f });
		transform->SetSize(static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
			static_cast<float>(ConfigManager::Instance().GetWindowHeight() - 100));
		AddGameObject(std::move(background));
	}

	void TankLevelScene::InitializeFPSCounter(SDL_Renderer* renderer)
	{
		auto fpsObject = std::make_unique<GameObject>("FPSCounter");
		fpsObject->AddComponent(std::make_unique<TextComponent>(
			"FPS 0", "data/fonts/tron-arcade.ttf", 10, SDL_Color{ 255, 255, 255, 255 }, renderer));
		fpsObject->AddComponent(std::make_unique<FPSComponent>(renderer));
		fpsObject->GetComponent<FPSComponent>()->Initialize();
		fpsObject->GetComponent<TransformComponent>()->SetPosition({ 10.f, 10.f });
		AddGameObject(std::move(fpsObject));
	}

	void TankLevelScene::InitializePlayer()
	{
		AddGameObject(PrefabRegistry::Instance().CreateRedTankPrefab(config.playerSpawn, "Player1"));
	}

	void TankLevelScene::InitializeUI()
	{
		AddGameObject(PrefabRegistry::Instance().CreateHealthUIForPlayer1({ 10.f, 30.f }, "HealthUIPlayer1"));
		AddGameObject(PrefabRegistry::Instance().CreateHighScoreUI({ 400.f, 30.f }, "HighScoreUI"));
		AddGameObject(PrefabRegistry::Instance().CreateCurrentScoreUI({ 750.f, 30.f }, "CurrentScoreUI"));
	}

	void TankLevelScene::InitializeWalls()
	{
		for (const auto& rect : FileReader(config.collisionPath).ReadRectangles())
		{
			auto wall = std::make_unique<GameObject>("Wall");
			auto collider = std::make_unique<BoxCollider>(rect);
			collider->isStatic = true;
			wall->AddComponent(std::move(collider));
			wall->GetComponent<TransformComponent>()->SetPosition({ rect.x, rect.y });
			wall->GetComponent<TransformComponent>()->SetPivot({ 0.f, 0.f });
			AddGameObject(std::move(wall));
		}
	}

	void TankLevelScene::InitializeManagers()
	{
		auto enemyManager = PrefabRegistry::Instance().CreateEnemyManager();
		auto* manager = enemyManager->GetComponent<EnemyManagerComponent>();
		manager->SetMaxBlueTanks(config.maxBlueTanks);
		manager->SetMaxPinkTanks(config.maxPinkTanks);
		manager->SetMaxRecognizers(config.maxRecognizers);
		manager->SetSpawnCooldown(config.spawnCooldown);
		AddGameObject(std::move(enemyManager));
	}

	void TankLevelScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();
		if (auto* player = FindGameObjectByTag("Player1"))
		{
			InputBindingHelper::BindSoloModeControls(player);
		}
	}

	void TankLevelScene::InitializeSounds()
	{
		auto& soundSystem = ServiceLocator::GetSoundSystem();
		soundSystem.AddSound(config.musicPath, SoundId::Music, true);
		SoundHelper::LoadSharedSounds();
		soundSystem.PlaySound(SoundId::Music, soundSystem.GetCurrentVolume());
	}

	void TankLevelScene::HandleInput(SDL_Event& event)
	{
		InputHandler::Instance().HandleInput(event);
	}

	void TankLevelScene::OnExit()
	{
		GameAdmin::Instance().ResetPlayers();
	}
}
