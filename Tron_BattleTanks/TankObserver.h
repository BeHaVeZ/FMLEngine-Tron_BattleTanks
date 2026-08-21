#pragma once
#include "Observer.h"
#include "Component.h"
#include "GameObjectDestroyedEvent.h"
#include "GameAdmin.h"
#include "GameData.h"
#include "GameTags.h"
#include "HealthComponent.h"
#include "ScoreComponent.h"
#include "ScreenShake.h"
#include <string>

namespace FML
{
	class TankObserver : public Component, public Observer
	{
	public:
		void HandleEvent(const Event& event) override
		{
			auto* destroyEvent = dynamic_cast<const GameObjectDestroyedEvent*>(&event);
			if (!destroyEvent)
				return;

			GameObject* destroyed = destroyEvent->GetDestroyedObject();
			if (Tags::IsEnemyTag(destroyed->GetTag()))
			{
				const int killScore = Tags::ScoreForTag(destroyed->GetTag());

				GameData::CurrentScore += killScore;
				CreditKiller(destroyed, killScore);

				const glm::vec2 deathPosition = destroyed->GetComponent<TransformComponent>()->GetWorldPosition();

				ScreenShake::Instance().TriggerNpcDeath();

				auto explosion = PrefabRegistry::Instance().CreateTankExplosionPrefab(deathPosition);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));

				auto floatingScore = PrefabRegistry::Instance().CreateFloatingScorePrefab(deathPosition, killScore);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(floatingScore));
			}
			else if (Tags::IsPlayerTag(destroyed->GetTag()))
			{
				GameAdmin::Instance().OnPlayerDestroyed(destroyed);

				ScreenShake::Instance().TriggerPlayerDeath();

				auto explosion = PrefabRegistry::Instance().CreatePlayerExplosionPrefab(destroyed->GetComponent<TransformComponent>()->GetWorldPosition());
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));

				SoundHelper::PlayRandomSound({ SoundId::PlayerExplosion1, SoundId::PlayerExplosion2 }, .3f);
			}
		};

	private:
		static void CreditKiller(GameObject* destroyed, int killScore)
		{
			const auto* health = destroyed->GetComponent<HealthComponent>();
			if (!health)
				return;

			const std::string_view killerTag = Tags::TagForPlayerNumber(health->GetLastAttacker());
			if (killerTag.empty())
				return;

			Scene* scene = SceneManager::Instance().GetCurrentScene();
			if (!scene)
				return;

			GameObject* killer = scene->FindGameObjectByTag(std::string(killerTag));
			if (!killer)
				return;

			if (auto* score = killer->GetComponent<ScoreComponent>())
			{
				score->AddKill(destroyed->GetTag(), killScore);
			}
		}
	};
}
