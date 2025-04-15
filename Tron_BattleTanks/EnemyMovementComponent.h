#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "SceneManager.h"
#include "CollisionManager.h"
#include "Timer.h"
#include <glm.hpp>
#include <random>
#include <cmath>

namespace FML
{
	class EnemyMovementComponent : public Component
	{
	public:
		explicit EnemyMovementComponent(float speed)
			: moveSpeed(speed)
		{
			std::random_device rd;
			rng = std::mt19937(rd());
			flipCoin = std::uniform_int_distribution<int>(0, 1);
			turnCooldown = 0.f;
		}

		void Update(float deltaTime) override
		{
			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture) return;

			glm::vec2 position = transform->GetWorldPosition();
			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 center = texture->GetWorldCenter();
			float rotation = transform->GetWorldRotation();

			// Move forward
			glm::vec2 newPos = position + forward * moveSpeed * deltaTime;
			transform->SetPosition(newPos);

			turnCooldown -= deltaTime;

			bool wallAhead = CollisionManager::Instance().RaycastWithTag(center, forward, 30.f, "Wall");

			if (wallAhead)
			{
				DecideTurn(transform, center, rotation);
			}
			else if (turnCooldown <= 0.f)
			{
				TryRandomTurn(transform, center, forward, rotation);
			}
		}

	private:
		float moveSpeed;
		float turnCooldown;
		std::mt19937 rng;
		std::uniform_int_distribution<int> flipCoin;

		glm::vec2 RotateVector(const glm::vec2& vec, float degrees)
		{
			float radians = glm::radians(degrees);
			float cosTheta = std::cos(radians);
			float sinTheta = std::sin(radians);
			return {
				vec.x * cosTheta - vec.y * sinTheta,
				vec.x * sinTheta + vec.y * cosTheta
			};
		}

		void TryRandomTurn(TransformComponent* transform, const glm::vec2& center, const glm::vec2& forward, float rotation)
		{
			glm::vec2 left = RotateVector(forward, 90.f);
			glm::vec2 right = RotateVector(forward, -90.f);

			bool leftFree = !CollisionManager::Instance().RaycastWithTag(center, left, 30.f, "Wall");
			bool rightFree = !CollisionManager::Instance().RaycastWithTag(center, right, 30.f, "Wall");

			if (leftFree && rightFree)
			{
				bool turnLeft = flipCoin(rng) == 0;
				transform->SetRotation(rotation + (turnLeft ? 90.f : -90.f));
				turnCooldown = 1.2f;
			}
			else if (leftFree)
			{
				transform->SetRotation(rotation + 90.f);
				turnCooldown = 1.2f;
			}
			else if (rightFree)
			{
				transform->SetRotation(rotation - 90.f);
				turnCooldown = 1.2f;
			}
		}

		void DecideTurn(TransformComponent* transform, const glm::vec2& center, float rotation)
		{
			glm::vec2 forward = {
				std::cos(glm::radians(rotation + 90.f)),
				std::sin(glm::radians(rotation + 90.f))
			};

			glm::vec2 left = RotateVector(forward, 90.f);
			glm::vec2 right = RotateVector(forward, -90.f);

			bool leftFree = !CollisionManager::Instance().RaycastWithTag(center, left, 30.f, "Wall");
			bool rightFree = !CollisionManager::Instance().RaycastWithTag(center, right, 30.f, "Wall");

			if (leftFree && !rightFree)
			{
				transform->SetRotation(rotation + 90.f);
				turnCooldown = 1.2f;
			}
			else if (!leftFree && rightFree)
			{
				transform->SetRotation(rotation - 90.f);
				turnCooldown = 1.2f;
			}
			else
			{
				transform->SetRotation(rotation + 180.f);
				turnCooldown = 1.2f;
			}
		}
	};
}
