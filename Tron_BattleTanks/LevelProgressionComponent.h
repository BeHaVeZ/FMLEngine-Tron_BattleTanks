#pragma once
#include "Component.h"
#include "GameData.h"
#include "GameObjectDestroyedEvent.h"
#include "GameTags.h"
#include "Logger.h"
#include "Observer.h"
#include "SceneManager.h"
#include <random>

namespace FML
{
	class LevelProgressionComponent final : public Component, public Observer
	{
	public:
		LevelProgressionComponent()
			: rng(std::random_device{}())
		{
		}

		void HandleEvent(const Event& event) override
		{
			const auto* destroyEvent = dynamic_cast<const GameObjectDestroyedEvent*>(&event);
			if (!destroyEvent || !destroyEvent->GetDestroyedObject()
				|| !Tags::IsEnemyTag(destroyEvent->GetDestroyedObject()->GetTag()))
			{
				return;
			}

			--remainingEnemies;
			Logger::Log(LogLevel::Info, "Enemy destroyed. Remaining enemies to kill: %d", remainingEnemies);
			if (remainingEnemies > 0 || GameData::CurrentGameMode != GameData::GameMode::Solo)
			{
				return;
			}

			auto& sceneManager = SceneManager::Instance();
			const std::string& currentSceneName = sceneManager.GetSceneName();
			if (currentSceneName == "Level404")
			{
				sceneManager.QueueSceneChange("Solo");
				return;
			}

			std::uniform_int_distribution<int> distribution(1, 100);
			if (distribution(rng) <= SecretLevelChancePercent)
			{
				Logger::Log(LogLevel::Info, "Level404 triggered");
				sceneManager.QueueSceneChange("Level404");
				return;
			}

			const Scene* nextScene = sceneManager.GetNextScene();
			if (!nextScene || nextScene->GetName() == "Coop" || nextScene->GetName() == "Versus")
			{
				sceneManager.QueueSceneChange("Solo");
			}
			else
			{
				sceneManager.QueueSceneChange(nextScene->GetName());
			}
		}

	private:
		static constexpr int SecretLevelChancePercent = 25;
		int remainingEnemies{ 10 };
		std::mt19937 rng;
	};
}
