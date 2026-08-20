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
				if (invulnerable)
					return;

				CurrentHealth() -= damageEvent->GetDamage();
				if (CurrentHealth() <= 0 && gameObject)
				{
					gameObject->Destroy();
					return;
				}
			}
		}

		void Update(float deltaTime) override
		{
			if (invulnerabilityTimer > 0.f)
				invulnerabilityTimer -= deltaTime;
		}

		void Damage(int damage)
		{
			if (invulnerabilityTimer > 0.f)
				return;

			gameObject->GetSubject().Notify(DamageEvent(damage));
		}

		int GetCurrentHealth() const
		{
			return sharedHealth ? *sharedHealth : health;
		}

		void SetInvulnerable(bool value) { invulnerable = value; }
		bool IsInvulnerable() const { return invulnerable; }

		void MakeInvulnerableFor(float seconds) { invulnerabilityTimer = seconds; }
		bool IsInInvulnerabilityWindow() const { return invulnerabilityTimer > 0.f; }

	private:
		int& CurrentHealth() { return sharedHealth ? *sharedHealth : health; }

		int health{ 0 };
		int* sharedHealth{ nullptr };
		bool invulnerable{ false };
		float invulnerabilityTimer{ 0.f };
	};
}