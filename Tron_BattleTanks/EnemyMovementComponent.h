#pragma once

#include "Component.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
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
			: moveSpeed(speed),
			turnCooldown(0.f),
			checkDistance(50.f),
			offsetDistance(4.f),
			turnCooldownTime(2.f)
		{
			std::random_device rd;
			rng = std::mt19937(rd());
			flipCoinDistribution = std::uniform_int_distribution<int>(0, 1);
		}

		void Update(float deltaTime) override
		{
			MoveUp();

			auto* transform = gameObject->GetComponent<TransformComponent>();
			auto* texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
				return;

			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };
			glm::vec2 center = transform->GetWorldPosition();

			float halfWidth = texture->GetDefaultWidth() * 0.5f;
			float halfHeight = texture->GetDefaultHeight() * 0.5f;

			glm::vec2 bottomLeft = center - right * halfWidth + up * halfHeight;
			glm::vec2 topLeft = center - right * halfWidth - up * halfHeight;
			glm::vec2 bottomRight = center + right * halfWidth + up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth - up * halfHeight;

			const float rayLength = checkDistance;

			DebugDraw::DrawLine(topLeft, topLeft - right * rayLength, { 1, 1, 0, 1 });
			DebugDraw::DrawLine(bottomLeft, bottomLeft - right * rayLength, { 1, 1, 0, 1 });
			DebugDraw::DrawLine(topRight, topRight + right * rayLength, { 1, 0, 1, 1 });
			DebugDraw::DrawLine(bottomRight, bottomRight + right * rayLength, { 1, 0, 1, 1 });
			DebugDraw::DrawLine(center, center - up * 20.f, { 0, 1, 1, 1 });

			bool wallFront = CollisionManager::Instance().RaycastWithTag(center, center - up, 10.f, "Wall");

			if (wallFront && turnCooldown <= 0.f)
			{
				Logger::Log(LogLevel::Debug, "Wall detected! Turning...");

				bool turnLeft = flipCoinDistribution(rng) == 0;

				if (turnLeft)
				{
					transform->SetRotation(worldRotationDegrees + 90.f);
				}
				else
				{
					transform->SetRotation(worldRotationDegrees - 90.f);
				}

				turnCooldown = turnCooldownTime;
			}

			turnCooldown -= deltaTime;
		}


		void Render(SDL_Renderer* renderer) override {}

	private:
		float moveSpeed;
		float turnCooldown;
		float turnCooldownTime;
		float checkDistance;
		float offsetDistance;
		std::mt19937 rng;
		std::uniform_int_distribution<int> flipCoinDistribution;

		void MoveUp()
		{
			auto* transform = gameObject->GetComponent<TransformComponent>();
			auto* texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
			{
				return;
			}

			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };

			glm::vec2 currentPos = transform->GetLocalPosition();
			glm::vec2 newPos = currentPos - up * moveSpeed * Timer::Instance().GetDeltaTime();
			transform->SetPosition(newPos);
		}

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
	};
}