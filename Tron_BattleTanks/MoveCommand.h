#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include <iostream>

namespace FML
{
	class MoveCommand : public Command
	{
	public:
		MoveCommand(GameObject* object, glm::vec2 direction, float distance)
			: gameObject(object), direction(glm::normalize(direction)), moveDistance(distance)
		{
		}

		void Execute() override
		{
			GameObject* object = gameObject.Get();
			if (object && !object->IsMarkedForDestruction())
			{
				auto transform = object->GetComponent<TransformComponent>();
				if (transform && !transform->IsMoving())
				{
					glm::vec2 newPosition = transform->GetLocalPosition() + direction * moveDistance * Timer::Instance().GetDeltaTime();
					transform->SetPosition(newPosition);
					transform->MarkMoving(true);

					if (glm::length(direction) > 0.0f)
					{
						float angleRadians = atan2(-direction.y, direction.x);
						float angleDegrees = glm::degrees(angleRadians);
						transform->SetRotation(angleDegrees - 90.0f);
					}
				}
			}
		}

	private:
		GameObjectRef gameObject;
		glm::vec2 direction;
		float moveDistance;
	};
}
