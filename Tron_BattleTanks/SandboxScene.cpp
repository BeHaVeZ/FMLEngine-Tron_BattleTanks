#include "SandboxScene.h"
#include "AIDifficultyProfile.h"
#include "AITankControllerComponent.h"
#include "ConfigManager.h"
#include "EnemyManagerComponent.h"
#include "GameData.h"
#include "GameTags.h"
#include "InputBindingHelper.h"
#include "InputHandler.h"
#include "NavGrid.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include <string>

namespace FML
{
	namespace
	{
		constexpr const char* SpawnLabelTag = "SpawnTypeUI";
		constexpr const char* InfoLabelTag = "SandboxInfoUI";
		constexpr const char* HudFontPath = "data/fonts/tron-arcade.ttf";
	}

	SandboxScene::SandboxScene()
		: TankLevelScene("Sandbox", LevelConfig{
			maps[0].backgroundPath, maps[0].collisionPath, "",
			{ 514.f, 428.f } })
	{
	}

	bool SandboxScene::Initialize(SDL_Renderer* renderer)
	{
		GameData::CurrentGameMode = GameData::GameMode::Coop;
		GameData::Player2IsAI = true;
		return TankLevelScene::Initialize(renderer);
	}

	void SandboxScene::InitializeUI()
	{
		auto* renderer = SceneManager::Instance().GetRenderer();

		auto spawnLabel = std::make_unique<GameObject>(SpawnLabelTag);
		spawnLabel->AddComponent(std::make_unique<TextComponent>(
			"SPAWN", HudFontPath, 10, SDL_Color{ 255, 255, 255, 255 }, renderer));
		spawnLabel->GetComponent<TransformComponent>()->SetPosition({ 10.f, 40.f });
		AddGameObject(std::move(spawnLabel));

		auto infoLabel = std::make_unique<GameObject>(InfoLabelTag);
		infoLabel->AddComponent(std::make_unique<TextComponent>(
			"MAP", HudFontPath, 10, SDL_Color{ 180, 180, 180, 255 }, renderer));
		infoLabel->GetComponent<TransformComponent>()->SetPosition({ 10.f, 58.f });
		AddGameObject(std::move(infoLabel));

		RefreshSpawnLabel();
		RefreshInfoLabel();
	}

	void SandboxScene::InitializeManagers()
	{
		auto enemyManager = PrefabRegistry::Instance().CreateEnemyManager();
		auto* manager = enemyManager->GetComponent<EnemyManagerComponent>();
		manager->SetMaxBlueTanks(config.maxBlueTanks);
		manager->SetMaxPinkTanks(config.maxPinkTanks);
		manager->SetMaxRecognizers(config.maxRecognizers);
		manager->SetSpawnCooldown(config.spawnCooldown);
		manager->SetSpawningEnabled(false);
		AddGameObject(std::move(enemyManager));
	}

	void SandboxScene::InitializeInput()
	{
		TankLevelScene::InitializeInput();

		auto& input = InputHandler::Instance();
		input.BindFunction(SDLK_LEFT, [this] { CycleSpawnType(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_RIGHT, [this] { CycleSpawnType(1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_u, [this] { CycleAllyDifficulty(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_TAB, [this] { CycleMap(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_g, [this] { ToggleAutoSpawn(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_t, [this] { SpawnWave(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_c, [this] { ClearNpcs(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_x, [this] { ClearBullets(); }, InputHandler::KeyAction::KeyUp);
	}

	void SandboxScene::HandleInput(SDL_Event& event)
	{
		TankLevelScene::HandleInput(event);

		if (event.type != SDL_MOUSEBUTTONDOWN)
		{
			return;
		}

		float x = 0.f;
		float y = 0.f;
		ConfigManager::Instance().WindowToLogical(event.button.x, event.button.y, x, y);
		if (y < static_cast<float>(hudHeight))
		{
			return;
		}

		if (event.button.button == SDL_BUTTON_RIGHT)
		{
			SpawnAt({ x, y });
		}
		else if (event.button.button == SDL_BUTTON_LEFT)
		{
			TeleportPlayerTo({ x, y });
		}
	}

	void SandboxScene::CycleSpawnType(int direction)
	{
		constexpr int typeCount = static_cast<int>(SpawnType::Explosion) + 1;
		const int index = (static_cast<int>(selectedSpawnType) + direction + typeCount) % typeCount;
		selectedSpawnType = static_cast<SpawnType>(index);
		RefreshSpawnLabel();
	}

	void SandboxScene::CycleAllyDifficulty()
	{
		switch (GameData::AiDifficulty)
		{
		case GameData::AIDifficulty::Easy:
			GameData::AiDifficulty = GameData::AIDifficulty::Normal;
			break;
		case GameData::AIDifficulty::Normal:
			GameData::AiDifficulty = GameData::AIDifficulty::Hard;
			break;
		case GameData::AIDifficulty::Hard:
			GameData::AiDifficulty = GameData::AIDifficulty::Easy;
			break;
		}
		RefreshSpawnLabel();
	}

	void SandboxScene::CycleMap()
	{
		mapIndex = (mapIndex + 1) % maps.size();
		config.backgroundPath = maps[mapIndex].backgroundPath;
		config.collisionPath = maps[mapIndex].collisionPath;
		SceneManager::Instance().ReloadScene();
	}

	void SandboxScene::ToggleAutoSpawn()
	{
		auto* managerObject = FindGameObjectByTag("EnemyManager");
		if (!managerObject)
		{
			return;
		}

		auto* manager = managerObject->GetComponent<EnemyManagerComponent>();
		manager->SetSpawningEnabled(!manager->IsSpawningEnabled());
		RefreshInfoLabel();
	}

	void SandboxScene::SpawnWave()
	{
		glm::vec2 origin = config.playerSpawn;
		if (auto* player = FindGameObjectByTag(std::string(Tags::Player1)))
		{
			origin = player->GetComponent<TransformComponent>()->GetWorldPosition();
		}

		auto& prefabs = PrefabRegistry::Instance();
		auto& grid = NavGrid::Instance();

		const auto spawnMany = [&](int count, auto&& create)
			{
				for (int i = 0; i < count; ++i)
				{
					glm::vec2 position{ 0.f, 0.f };
					if (grid.FindRandomGoal(origin, tankRadius, 150.f, position))
					{
						AddGameObject(create(position));
					}
				}
			};

		spawnMany(config.maxBlueTanks, [&](const glm::vec2& position) { return prefabs.CreateBlueTankPrefab(position); });
		spawnMany(config.maxPinkTanks, [&](const glm::vec2& position) { return prefabs.CreatePinkTankPrefab(position); });
		spawnMany(config.maxRecognizers, [&](const glm::vec2& position) { return prefabs.CreateRecognizerPrefab(position); });
	}

	void SandboxScene::RefreshSpawnLabel()
	{
		auto* label = FindGameObjectByTag(SpawnLabelTag);
		if (!label)
		{
			return;
		}

		std::string text = "SPAWN ";
		switch (selectedSpawnType)
		{
		case SpawnType::BlueTank:
			text += "BLUE TANK";
			break;
		case SpawnType::PinkTank:
			text += "PINK TANK";
			break;
		case SpawnType::Recognizer:
			text += "RECOGNIZER";
			break;
		case SpawnType::AIAlly:
			text += std::string("AI ALLY ") + DifficultyName(GameData::AiDifficulty);
			break;
		case SpawnType::TargetDummy:
			text += "TARGET DUMMY";
			break;
		case SpawnType::EnemyBullet:
			text += "ENEMY BULLET";
			break;
		case SpawnType::Explosion:
			text += "EXPLOSION FX";
			break;
		}
		text += "   ARROWS CHANGE   RMB PLACE";

		label->GetComponent<TextComponent>()->SetText(text, SceneManager::Instance().GetRenderer());
	}

	void SandboxScene::RefreshInfoLabel()
	{
		auto* label = FindGameObjectByTag(InfoLabelTag);
		if (!label)
		{
			return;
		}

		bool autoSpawnOn = false;
		if (auto* managerObject = FindGameObjectByTag("EnemyManager"))
		{
			autoSpawnOn = managerObject->GetComponent<EnemyManagerComponent>()->IsSpawningEnabled();
		}

		const std::string text = std::string("MAP ") + maps[mapIndex].name + " TAB   AUTO SPAWN "
			+ (autoSpawnOn ? "ON" : "OFF") + " G   WAVE T";
		label->GetComponent<TextComponent>()->SetText(text, SceneManager::Instance().GetRenderer());
	}

	void SandboxScene::SpawnAt(const glm::vec2& position)
	{
		auto& prefabs = PrefabRegistry::Instance();

		glm::vec2 snapped = position;
		NavGrid::Instance().FindNearestWalkable(position, tankRadius, snapped);

		switch (selectedSpawnType)
		{
		case SpawnType::BlueTank:
			AddGameObject(prefabs.CreateBlueTankPrefab(snapped, std::string(Tags::BlueTank)));
			break;
		case SpawnType::PinkTank:
			AddGameObject(prefabs.CreatePinkTankPrefab(snapped, std::string(Tags::PinkTank)));
			break;
		case SpawnType::Recognizer:
			AddGameObject(prefabs.CreateRecognizerPrefab(snapped, std::string(Tags::Recognizer)));
			break;
		case SpawnType::AIAlly:
		{
			if (auto* existing = FindGameObjectByTag(std::string(Tags::Player2)))
			{
				existing->Destroy();
			}

			auto ally = prefabs.CreateYellowTankPrefab(snapped, std::string(Tags::Player2));
			ally->AddComponent(std::make_unique<AITankControllerComponent>(GameData::AiDifficulty));
			ally->GetComponent<AITankControllerComponent>()->Initialize();
			AddGameObject(std::move(ally));
			break;
		}
		case SpawnType::TargetDummy:
			AddGameObject(prefabs.CreateTargetDummyPrefab(snapped));
			break;
		case SpawnType::EnemyBullet:
		{
			glm::vec2 direction{ 0.f, -1.f };
			if (auto* player = FindGameObjectByTag(std::string(Tags::Player1)))
			{
				const glm::vec2 toPlayer = player->GetComponent<TransformComponent>()->GetWorldPosition() - position;
				if (glm::length(toPlayer) > 1.f)
				{
					direction = glm::normalize(toPlayer);
				}
			}
			AddGameObject(prefabs.CreateEnemyBulletPrefab(position, direction, 250.f));
			break;
		}
		case SpawnType::Explosion:
			AddGameObject(prefabs.CreateTankExplosionPrefab(position));
			break;
		}
	}

	void SandboxScene::TeleportPlayerTo(const glm::vec2& position)
	{
		auto* player = FindGameObjectByTag(std::string(Tags::Player1));
		if (!player)
		{
			return;
		}

		glm::vec2 snapped = position;
		NavGrid::Instance().FindNearestWalkable(position, tankRadius, snapped);

		player->GetComponent<TransformComponent>()->SetPosition(snapped);
		AddGameObject(PrefabRegistry::Instance().CreateTpEffect(snapped));
	}

	void SandboxScene::ClearNpcs()
	{
		for (const auto& gameObject : gameObjects)
		{
			const std::string& tag = gameObject->GetTag();
			if (Tags::IsEnemyTag(tag) || tag == Tags::Player2)
			{
				gameObject->Destroy();
			}
		}
	}

	void SandboxScene::ClearBullets()
	{
		for (const auto& gameObject : gameObjects)
		{
			const std::string& tag = gameObject->GetTag();
			if (tag == Tags::Bullet || tag == Tags::EnemyBullet)
			{
				gameObject->Destroy();
			}
		}
	}
}
