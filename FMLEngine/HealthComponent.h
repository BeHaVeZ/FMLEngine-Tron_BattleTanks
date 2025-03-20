#pragma once
#include "Component.h"
#include "Observer.h"
#include "GameObject.h"
#include "Event.h"
#include "iostream"

class HealthComponent : public Component, public Observer
{
public:
	HealthComponent(int health) : health(health) {}

	void Damage(int damage)
	{
		health -= damage;
		Event event = Event::Damage(damage);
		gameObject->GetSubject().Notify(*gameObject, event);
	}

	void OnNotify(const GameObject& go, Event event) override
	{
		if (event.type == EventType::DamageTaken)
		{
			auto damageEvent = std::get<DamageEvent>(event.data);
			std::cout << "HealthComponent: " << go.GetTag() << " took " << damageEvent.damageAmount << " damage!" << std::endl;
		}
	}

private:
	int health;
};
