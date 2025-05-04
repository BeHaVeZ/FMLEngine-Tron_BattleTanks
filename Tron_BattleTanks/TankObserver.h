#pragma once
#include "Observer.h"
#include "Component.h"
#include "GameObjectDestroyedEvent.h"
#include "GameAdmin.h"
#include "Logger.h"

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
			if (destroyed->GetTag() == "Enemy")
			{
				auto player = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1");
				if (player)
				{
					player->GetComponent<ScoreComponent>()->AddScore();
				}
				gameObject->GetSubject().Notify(BlueTankKilledEvent());

				Logger::Log(LogLevel::Error, "Enemy destroyed: Spawning explosion at %d.", destroyed->GetComponent<TransformComponent>()->GetWorldPosition().x);

				auto explosion = PrefabRegistry::Instance().CreateTankExplosionPrefab(
					destroyed->GetComponent<TransformComponent>()->GetWorldPosition()
				);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));
			}
			else 	if (destroyed->GetTag() == "Player1" || destroyed->GetTag() == "Player2")
			{
				GameAdmin::Instance().OnPlayerDestroyed(destroyed);

				auto explosion = PrefabRegistry::Instance().CreatePlayerExplosionPrefab(
					destroyed->GetComponent<TransformComponent>()->GetWorldPosition()
				);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));

				Logger::Log(LogLevel::Error, "Player destroyed: Spawning explosion.");
			}
		};
	};
}
