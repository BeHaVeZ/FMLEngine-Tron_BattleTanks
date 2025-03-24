#pragma once
#include "Observer.h"
#include "Logger.h"

namespace FML
{

	class HealthComponent : public Component, public Observer
	{
	public:
		HealthComponent(int initialHealth) : health(initialHealth) {}

		void HandleEvent(const DamageEvent& event) override
		{
			health -= event.GetDamage();
			Logger::Log(LogLevel::Info, "HealthComponent::HandleEvent - Health changed to: %d", health);
		}

		void Damage(int damage)
		{
			gameObject->GetSubject().Notify(DamageEvent(damage));
		}

	private:
		int health;
	};
}