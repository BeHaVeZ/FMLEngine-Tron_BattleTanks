#pragma once
#include "Observer.h"
#include "DamageEvent.h"
#include "Logger.h"
#include "SceneManager.h"
#include "../Tron_BattleTanks/BlueTankKilledEvent.h"
#include "../Tron_BattleTanks/ScoreComponent.h"
#include "../Tron_BattleTanks/TeleportManager.h"

namespace FML
{
	class HealthComponent : public Component, public Observer
	{
	public:
		HealthComponent(int initialHealth) : health(initialHealth) {}

		void HandleEvent(const Event& event) override
		{
			if (const auto* damageEvent = dynamic_cast<const DamageEvent*>(&event)) {
				health -= damageEvent->GetDamage();
				if (gameObject && gameObject->GetTag() == "Player1" || gameObject && gameObject->GetTag() == "Player2")
				{
					gameObject->GetComponent<TransformComponent>()->SetPosition(TeleportManager::Instance().GetRandomTeleportPosition());
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