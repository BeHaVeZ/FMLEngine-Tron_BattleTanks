#pragma once
#include "Observer.h"
#include "DamageEvent.h"
#include "GameObject.h"

namespace FML
{
	class HealthComponent : public Component, public Observer
	{
	public:
		explicit HealthComponent(int initialHealth) : health(initialHealth) {}
		explicit HealthComponent(int* sharedHealth) : sharedHealth(sharedHealth) {}

		void HandleEvent(const Event& event) override
		{
			if (const auto* damageEvent = dynamic_cast<const DamageEvent*>(&event)) 
			{
				CurrentHealth() -= damageEvent->GetDamage();
				if (CurrentHealth() <= 0 && gameObject)
				{
					gameObject->Destroy();
					return;
				}
			}
		}

		void Damage(int damage)
		{
			gameObject->GetSubject().Notify(DamageEvent(damage));
		}

		int GetCurrentHealth() const
		{
			return sharedHealth ? *sharedHealth : health;
		}

	private:
		int& CurrentHealth() { return sharedHealth ? *sharedHealth : health; }

		int health{ 0 };
		int* sharedHealth{ nullptr };
	};
}