#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace FML
{
	class RotateTurretCommand : public Command {
	public:
		RotateTurretCommand(GameObject* gameObject, float rotationSpeed = 2000.f)
			: gameObject(gameObject), rotationSpeed(rotationSpeed)
		{
		}

		void Execute() override
		{
			if (gameObject) {
				auto transform = gameObject->GetComponent<TransformComponent>();
				if (transform) {
					float currentRotation = transform->GetLocalRotation();

					float deltaTime = Timer::Instance().GetDeltaTime();
					currentRotation += rotationSpeed * deltaTime;

					transform->SetRotation(currentRotation);
				}
			}
		}

	private:
		GameObject* gameObject;
		float rotationSpeed;
	};
}