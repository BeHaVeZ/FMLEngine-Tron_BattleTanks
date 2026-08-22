#pragma once
#include "Command.h"
#include "GameObject.h"
#include "Event.h"
#include "HealthComponent.h"
#include <iostream>

namespace FML
{
	class DamageCommand : public Command
	{
	public:
		DamageCommand(GameObject* gameObject, int damage) : gameObject(gameObject), damage(damage) {}

		void Execute() override {
			GameObject* object = gameObject.Get();
			if (object && !object->IsMarkedForDestruction())
			{
				HealthComponent* hc = object->GetComponent<HealthComponent>();
				if (hc)
				{
					hc->Damage(damage);
				}
			}
		}

	private:
		GameObjectRef gameObject;
		int damage;
	};
}


