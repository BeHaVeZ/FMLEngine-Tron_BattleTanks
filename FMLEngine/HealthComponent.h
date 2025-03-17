#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include <cmath>
#include "Observer.h"

class HealthComponent : public Component, public Observer
{
public:
	HealthComponent(int health) : health(health){};

	void OnNotify(const GameObject& gameObject, Event event) override;

	void Damage(int damage);

private:
	int health;
};
