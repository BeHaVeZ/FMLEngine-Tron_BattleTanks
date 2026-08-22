#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
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
			if (gameObject)
			{
				HealthComponent* hc = gameObject->GetComponent<HealthComponent>();
				if (hc)
				{
					hc->Damage(damage);
				}
			}
		}

	private:
		GameObject* gameObject;
		int damage;
	};
}


