#pragma once
#include "Observer.h"
#include "Component.h"
#include "GameObjectDestroyedEvent.h"
#include "GameAdmin.h"
#include "Logger.h"
#include "GameTags.h"

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
				auto player = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag(Tags::Player1.data());
				if (player)
				{
					if (auto* score = player->GetComponent<ScoreComponent>())
					{
						score->AddScore();
					}
				}
				gameObject->GetSubject().Notify(BlueTankKilledEvent());

				Logger::Log(LogLevel::Info, "Enemy destroyed: spawning explosion at %.1f.", destroyed->GetComponent<TransformComponent>()->GetWorldPosition().x);

				auto explosion = PrefabRegistry::Instance().CreateTankExplosionPrefab(destroyed->GetComponent<TransformComponent>()->GetWorldPosition());
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));
			}
			else if (Tags::IsPlayerTag(destroyed->GetTag()))
			{
				GameAdmin::Instance().OnPlayerDestroyed(destroyed);

				auto explosion = PrefabRegistry::Instance().CreatePlayerExplosionPrefab(destroyed->GetComponent<TransformComponent>()->GetWorldPosition());
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));

				SoundHelper::PlayRandomSound({ SoundId::PlayerExplosion1, SoundId::PlayerExplosion2 }, .3f);

				Logger::Log(LogLevel::Error, "Player destroyed: Spawning explosion.");
			}
		};
	};
}
