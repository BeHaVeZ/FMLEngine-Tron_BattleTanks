#include "HealthComponent.h"
#include "Event.h"
#include "iostream"

void HealthComponent::OnNotify(const GameObject&, Event event)
{
	if (event.type == EventType::DamageTaken and std::holds_alternative<DamageEvent>(event.data))
	{
		Damage(std::get<DamageEvent>(event.data).damageAmount);
	}
}

void HealthComponent::Damage(int damage)
{
	health -= damage;
	std::cout << "Health: " << health << std::endl;
	if (health <= 0)
	{
		//gameObject->Destroy();
	}
}
