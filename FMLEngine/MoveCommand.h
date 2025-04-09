#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include <iostream>
#include <glm.hpp>

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
			if (gameObject) 
			{
				auto transform = gameObject->GetComponent<TransformComponent>();
				if (transform && transform->IsMoving() == false) 
				{
					glm::vec2 newPosition = transform->GetLocalPosition() + direction * moveDistance * Timer::Instance().GetDeltaTime();
					transform->SetPosition(newPosition);

					float angleRadians = atan2(-direction.y, -direction.x);
					float angleDegrees = glm::degrees(angleRadians);

					transform->SetRotation(angleDegrees - 90);
				}
			}
		}

	private:
		GameObject* gameObject;
		glm::vec2 direction;
		float moveDistance;
	};
}


