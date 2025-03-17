#pragma once
#include "Command.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include <iostream>

class DamageCommand : public Command {
public:
    DamageCommand(GameObject* gameObject,int incomingDamage = 0) : gameObject(gameObject), damage(incomingDamage){}

    void Execute() override 
    {
        if (gameObject) 
        {
			gameObject->Notify(Event::Damage(damage));
        }
    }

private:
    GameObject* gameObject;
    int damage;
};
