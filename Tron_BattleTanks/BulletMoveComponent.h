#pragma once
#include "Component.h"
#include <glm.hpp>
#include "TransformComponent.h"


namespace FML
{
	class BulletMoveComponent : public Component
	{
	public:
		BulletMoveComponent(glm::vec2 moveDirection = {0,0}, float bulletSpeed = 30.f) : moveDirection(moveDirection), bulletSpeed(bulletSpeed) {};

		void Update(float deltaTime) override
		{
			if (gameObject)
			{
				auto transform = gameObject->GetComponent<TransformComponent>();
				glm::vec2 newPos = transform->GetLocalPosition() + moveDirection * bulletSpeed * deltaTime;
				transform->SetPosition(newPos);
			}
		}
	private:
		glm::vec2 moveDirection;
		float bulletSpeed;
	};
}