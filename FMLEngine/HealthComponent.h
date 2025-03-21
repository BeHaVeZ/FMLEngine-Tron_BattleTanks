#pragma once
#include "Observer.h"
#include <iostream>

class HealthComponent : public Component, public Observer {
public:
    HealthComponent(int initialHealth) : health(initialHealth) {}

    void HandleEvent(const DamageEvent& event) override 
    {
        health -= event.GetDamage();
        std::cout << "Health reduced to: " << health << std::endl;
    }

    void Damage(int damage) 
    {
		gameObject->GetSubject().Notify(DamageEvent(damage));
    }

private:
    int health;
};
