#pragma once

#include <vector>
#include <glm.hpp>
#include <random>
#include "Observer.h"
#include "GameObject.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "Logger.h"
#include "BlueTankKilledEvent.h"
#include <GameObjectDestroyedEvent.h>

namespace FML
{
	class EnemyManagerComponent : public Component, public Observer
	{
	public:
		enum class EnemyType { Blue, Pink, Recognizer };

		EnemyManagerComponent() :
			enemiesToKillForTheNextLevel(10),
			spawnCooldown(0.f),
			spawnCooldownTime(2.5f),
			visibilityTolerance(20.f),
			maxBlueTanks(3),
			maxPinkTanks(2),
			maxRecognizers(1),
			currentBlueTanks(0),
			currentPinkTanks(0),
			currentRecognizers(0)
		{
			std::random_device rd;
			rng = std::mt19937(rd());

			spawnPositions =
			{
				{956,133},
				{958,719},
				{71,133},
				{480,133},
				{71,351},
				{71,722},
				{480,722}
			};
		};

		void SetMaxRecognizers(int newMax) { maxRecognizers = newMax; }
		void SetMaxBlueTanks(int newMax) { maxBlueTanks = newMax; }
		void SetMaxPinkTanks(int newMax) { maxPinkTanks = newMax; }

		void SetSpawnCooldown(float newCooldown) { spawnCooldownTime = newCooldown; }

		void Update(float dt) override
		{
			spawnCooldown -= dt;
			if (spawnCooldown <= 0.f)
			{
				auto player1 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1");
				auto player2 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player2");
				if (player1 || player2)
				{
					if (player1)
					{
						SpawnEnemy(player1);
					}
					else
						SpawnEnemy(player2);
					spawnCooldown = spawnCooldownTime;
				}
			}
		}

		void SpawnEnemy(GameObject* player)
		{
			std::vector<EnemyType> availableTypes;

			if (currentBlueTanks < maxBlueTanks)
				availableTypes.push_back(EnemyType::Blue);

			if (currentPinkTanks < maxPinkTanks)
				availableTypes.push_back(EnemyType::Pink);

			if (currentRecognizers < maxRecognizers)
				availableTypes.push_back(EnemyType::Recognizer);

			if (availableTypes.empty())
				return;

			std::uniform_int_distribution<> dist(0, static_cast<int>(availableTypes.size() - 1));
			EnemyType chosenType = availableTypes[dist(rng)];

			glm::vec2 pos = ChooseSpawnPosition(player);
			std::unique_ptr<GameObject> enemy;

			switch (chosenType)
			{
			case EnemyType::Blue:
				enemy = PrefabRegistry::Instance().CreateBlueTankPrefab(pos);
				++currentBlueTanks;
				break;
			case EnemyType::Pink:
				enemy = PrefabRegistry::Instance().CreatePinkTankPrefab(pos);
				++currentPinkTanks;
				break;
			case EnemyType::Recognizer:
				enemy = PrefabRegistry::Instance().CreateRecognizerPrefab(pos);
				++currentRecognizers;
				break;
			}

			GameObject* rawEnemy = enemy.get();
			rawEnemy->GetSubject().AddObserver(this);

			SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(enemy));

			auto tpEffect = PrefabRegistry::Instance().CreateTpEffect(pos);
			SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(tpEffect));
		}

		glm::vec2 ChooseSpawnPosition(GameObject* player)
		{
			glm::vec2 playerPos = player->GetComponent<TransformComponent>()->GetWorldPosition();

			std::vector<glm::vec2> filtered;

			for (const auto& pos : spawnPositions)
			{
				bool xVisible = std::abs(pos.x - playerPos.x) < visibilityTolerance;
				bool yVisible = std::abs(pos.y - playerPos.y) < visibilityTolerance;

				if (!xVisible && !yVisible)
				{
					filtered.push_back(pos);
				}
			}

			if (filtered.empty())
			{
				filtered = spawnPositions;
			}

			std::uniform_int_distribution<> dist(0, static_cast<int>(filtered.size() - 1));
			return filtered[dist(rng)];
		};

		void HandleEvent(const Event& event) override
		{
			const GameObjectDestroyedEvent* destroyEvent = dynamic_cast<const GameObjectDestroyedEvent*>(&event);
			if (destroyEvent)
			{
				GameObject* destroyed = destroyEvent->GetDestroyedObject();
				if (!destroyed) return;

				std::string tag = destroyed->GetTag();

				if (tag == "BlueTank")
					--currentBlueTanks;
				else if (tag == "PinkTank")
					--currentPinkTanks;
				else if (tag == "Recognizer")
					--currentRecognizers;

				--enemiesToKillForTheNextLevel;
				Logger::Log(LogLevel::Warning, "Enemy destroyed. Remaining enemies to kill: %d", enemiesToKillForTheNextLevel);
			}

			
			if (enemiesToKillForTheNextLevel <= 0 && GameData::CurrentGameMode == GameData::GameMode::Solo)
			{
				std::string currentScene = SceneManager::Instance().GetCurrentScene()->GetName();

				if (currentScene == "Level404")
				{
					Logger::Log(LogLevel::Info, "Exiting Level404, returning to Solo.");
					SceneManager::Instance().QueueSceneChange("Solo");
					return;
				}

				std::random_device rd;
				std::mt19937 localRng(rd());
				std::uniform_int_distribution<> dist(1, 100);

				int roll = dist(localRng);

				if (roll <= 25)
				{
					Logger::Log(LogLevel::Error, "Level404 triggered!");
					SceneManager::Instance().QueueSceneChange("Level404");
				}
				else
				{
					auto& nextSceneName = SceneManager::Instance().GetNextScene()->GetName();
					if (nextSceneName == "Coop" || nextSceneName == "Versus")
						SceneManager::Instance().QueueSceneChange("Solo");
					else
						SceneManager::Instance().QueueSceneChange(nextSceneName);
				}
			}
		}

	private:
		std::vector<glm::vec2> spawnPositions;

		int maxBlueTanks;
		int maxPinkTanks;
		int maxRecognizers;

		int currentBlueTanks;
		int currentPinkTanks;
		int currentRecognizers;

		int enemiesToKillForTheNextLevel;

		float spawnCooldown;
		float spawnCooldownTime;

		float visibilityTolerance;
		std::mt19937 rng;

	};
}
