#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "ShootComponent.h"
#include <iostream>

namespace FML
{
	class ShootCommand : public Command
	{
	public:
		ShootCommand(GameObject* gameObject) : gameObject(gameObject) {}

		void Execute() override
		{
			GameObject* object = gameObject.Get();
			if (object && !object->IsMarkedForDestruction())
			{
				if (auto shoot = object->GetComponent<ShootComponent>())
					shoot->Shoot();
			}
		}

	private:
		GameObjectRef gameObject;
	};
}
