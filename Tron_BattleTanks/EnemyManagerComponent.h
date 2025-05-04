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

namespace FML
{
	class EnemyManagerComponent : public Component, public Observer
	{
	public:
		EnemyManagerComponent() :
			maxEnemies(5),
			currentEnemies(0),
			enemiesToKillForTheNextLevel(10),
			spawnCooldown(0.f),
			spawnCooldownTime(2.5f),
			visibilityTolerance(20.f)
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

		void Update(float dt) override
		{
			spawnCooldown -= dt;
			if (spawnCooldown <= 0.f)
			{
				auto player = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1");
				if (player)
				{
					SpawnEnemy(player);
					spawnCooldown = spawnCooldownTime;
				}
			}
		}

		void SpawnEnemy(GameObject* player)
		{
			if (currentEnemies >= maxEnemies)
				return;

			glm::vec2 pos = ChooseSpawnPosition(player);
			auto enemy = PrefabRegistry::Instance().CreateBlueTankPrefab(pos);

			GameObject* rawEnemy = enemy.get();
			rawEnemy->GetSubject().AddObserver(this);

			SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(enemy));
			auto tpEffect = PrefabRegistry::Instance().CreateTpEffect(pos);
			SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(tpEffect));
			++currentEnemies;
		};
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
			if (const BlueTankKilledEvent* blueTankKilledEvent = dynamic_cast<const BlueTankKilledEvent*>(&event))
			{
				currentEnemies--;
				enemiesToKillForTheNextLevel--;
				Logger::Log(LogLevel::Warning, "CurrentEnemies is %d", currentEnemies);
			}

			if (enemiesToKillForTheNextLevel <= 0 && GameData::CurrentGameMode == GameData::GameMode::Solo)
			{
				auto& nextSceneName = SceneManager::Instance().GetNextScene()->GetName();
				if (nextSceneName == "Coop" || nextSceneName == "Versus")
				{
					SceneManager::Instance().QueueSceneChange("Solo");
				}
				else
					SceneManager::Instance().QueueSceneChange(SceneManager::Instance().GetNextScene()->GetName());
			}
		};

	private:
		std::vector<glm::vec2> spawnPositions;
		float visibilityTolerance;
		std::mt19937 rng;
		int maxEnemies;
		int currentEnemies;
		int enemiesToKillForTheNextLevel;
		float spawnCooldown;
		float spawnCooldownTime;
	};
}
