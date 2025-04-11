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
			if (gameObject && !gameObject->IsMarkedForDestruction())
			{
				gameObject->GetComponent<ShootComponent>()->Shoot();
			}
		}

	private:
		GameObject* gameObject;
	};
}
