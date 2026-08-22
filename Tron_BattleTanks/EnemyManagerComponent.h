#pragma once

#include <vector>
#include <glm.hpp>
#include <random>
#include "Observer.h"
#include "GameObject.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "Logger.h"
#include "GameObjectDestroyedEvent.h"
#include "GameTags.h"
#include "LevelProgressionComponent.h"

namespace FML
{
	class EnemyManagerComponent : public Component, public Observer
	{
	public:
		enum class EnemyType { Blue, Pink, Recognizer };

		EnemyManagerComponent() :
			spawnPositions{
				{956,133},
				{958,719},
				{71,133},
				{480,133},
				{71,351},
				{71,722},
				{480,722}
			},
			maxBlueTanks(3),
			maxPinkTanks(2),
			maxRecognizers(1),
			currentBlueTanks(0),
			currentPinkTanks(0),
			currentRecognizers(0),
			spawnCooldown(0.f),
			spawnCooldownTime(2.5f),
			visibilityTolerance(20.f),
			rng(std::random_device{}())
		{
		};

		void SetMaxRecognizers(int newMax) { maxRecognizers = newMax; }
		void SetMaxBlueTanks(int newMax) { maxBlueTanks = newMax; }
		void SetMaxPinkTanks(int newMax) { maxPinkTanks = newMax; }

		void SetSpawnCooldown(float newCooldown) { spawnCooldownTime = newCooldown; }

		void SetSpawningEnabled(bool enabled) { spawningEnabled = enabled; }
		bool IsSpawningEnabled() const { return spawningEnabled; }

		void Update(float dt) override
		{
			if (!spawningEnabled)
				return;

			spawnCooldown -= dt;
			if (spawnCooldown <= 0.f)
			{
				auto player1 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag(Tags::Player1.data());
				auto player2 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag(Tags::Player2.data());
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
			if (auto* progression = gameObject->GetComponent<LevelProgressionComponent>())
			{
				rawEnemy->GetSubject().AddObserver(progression);
			}

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

				const std::string& tag = destroyed->GetTag();

				if (tag == Tags::BlueTank)
					--currentBlueTanks;
				else if (tag == Tags::PinkTank)
					--currentPinkTanks;
				else if (tag == Tags::Recognizer)
					--currentRecognizers;

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

		float spawnCooldown;
		float spawnCooldownTime;

		bool spawningEnabled{ true };

		float visibilityTolerance;
		std::mt19937 rng;

	};
}
