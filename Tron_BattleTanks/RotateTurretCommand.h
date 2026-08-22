#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TurretAimComponent.h"
#include "Timer.h"

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
			GameObject* object = gameObject.Get();
			if (!object || object->IsMarkedForDestruction()) return;

			const float delta = rotationSpeed * Timer::Instance().GetDeltaTime();

			if (auto* aim = object->GetComponent<TurretAimComponent>())
			{
				aim->Rotate(delta);
				return;
			}

			auto transform = object->GetComponent<TransformComponent>();
			if (transform)
			{
				transform->SetRotation(transform->GetLocalRotation() + delta);
			}
		}

	private:
		GameObjectRef gameObject;
		float rotationSpeed;
	};
}
