#pragma once
#include "Observer.h"
#include "DamageEvent.h"
#include "Logger.h"
#include "SceneManager.h"
#include "../Tron_BattleTanks/BlueTankKilledEvent.h"
#include "../Tron_BattleTanks/ScoreComponent.h"
#include "../Tron_BattleTanks/TeleportManager.h"
#include "PrefabRegistry.h"

namespace FML
{
	class HealthComponent : public Component, public Observer
	{
	public:
		HealthComponent(int initialHealth) : health(initialHealth) {}

		void HandleEvent(const Event& event) override
		{
			if (const auto* damageEvent = dynamic_cast<const DamageEvent*>(&event)) 
			{
				health -= damageEvent->GetDamage();
				auto transform = gameObject->GetComponent<TransformComponent>();
				if (gameObject && gameObject->GetTag() == "Player1" && health > 0 || gameObject && gameObject->GetTag() == "Player2" && health > 0)
				{
					auto explosion = PrefabRegistry::Instance().CreateTankExplosionPrefab(
						transform->GetWorldPosition()
					);
					SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));
					transform->SetPosition(TeleportManager::Instance().GetRandomTeleportPosition());
					transform->UpdateWorldPosition();

					auto tpEffect = PrefabRegistry::Instance().CreateTpEffect(transform->GetWorldPosition());
					SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(tpEffect));
				}
				if (health <= 0)
				{
					gameObject->Destroy();
					return;
				}
				Logger::Log(LogLevel::Info, "HealthComponent::HandleEvent - Health changed to: %d", health);
			}
		}

		void Damage(int damage)
		{
			gameObject->GetSubject().Notify(DamageEvent(damage));
		}

		int GetCurrentHealth() const
		{
			return health;
		}

	private:
		int health;
	};
}