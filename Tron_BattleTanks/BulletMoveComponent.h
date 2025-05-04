#pragma once
#include "Component.h"
#include <glm.hpp>
#include "TransformComponent.h"
#include "SoundHelper.h"


namespace FML
{
	class BulletMoveComponent : public Component
	{
	public:
		BulletMoveComponent(glm::vec2 moveDirection = {0,0}, float bulletSpeed = 30.f, int maxBounces = 4) : moveDirection(moveDirection), bulletSpeed(bulletSpeed), bounceCount(0),maxBounces(maxBounces) {};

		void Update(float deltaTime) override
		{
			if (gameObject)
			{
				auto transform = gameObject->GetComponent<TransformComponent>();
				glm::vec2 newPos = transform->GetLocalPosition() + moveDirection * bulletSpeed * deltaTime;
				transform->SetPosition(newPos);
			}
		}

		void Bounce(const glm::vec2& normal)
		{
			SoundHelper::PlayRandomSound({ 2,3,4 }, .3f);
			if (bounceCount == maxBounces)
			{
				gameObject->Destroy();
				return;
			}
			moveDirection = glm::reflect(moveDirection, glm::normalize(normal));
			++bounceCount;
		}

		glm::vec2 GetMoveDirection() const { return moveDirection; }
		void SetMoveDirection(const glm::vec2& dir) { moveDirection = dir; }

	private:
		glm::vec2 moveDirection;
		float bulletSpeed;
		int bounceCount;
		int maxBounces;
	};
}