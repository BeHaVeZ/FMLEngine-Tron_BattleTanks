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
			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
			{
				return;
			}

			glm::vec2 center = texture->GetWorldCenter();
			glm::vec2 position = transform->GetWorldPosition();
			glm::vec2 forward = texture->GetForwardVector();
			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };

			float halfWidth = texture->GetDefaultWidth() / 2.f;
			float halfHeight = texture->GetDefaultHeight() / 2.f;
			glm::vec2 topLeft = center - right * halfWidth - up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth - up * halfHeight;

			glm::vec2 newPosition = position + forward * moveSpeed * deltaTime;
			transform->SetPosition(newPosition);

			turnCooldown -= deltaTime;

			float shortRayLength = -5.f; // Ray length for immediate obstacle detection.
			float longRayLength = checkDistance; // Ray length for detecting entities ahead.

			//DebugDraw::DrawLine(topLeft, topLeft + up * shortRayLength, { 1, 0, 0, 1 });		// Red (Top-Left)
			//DebugDraw::DrawLine(topRight, topRight + up * shortRayLength, { 0, 1, 0, 1 });	// Green (Top-Right)
			//DebugDraw::DrawLine(center, center + forward * longRayLength, { 1, 1, 0, 1 }); // Yellow (Center Forward)

			bool wallAheadTopLeft = CollisionManager::Instance().RaycastWithTag(topLeft, up, shortRayLength, "Wall");
			bool wallAheadTopRight = CollisionManager::Instance().RaycastWithTag(topRight, up, shortRayLength, "Wall");
			bool wallAheadCenter = CollisionManager::Instance().RaycastWithTag(center, forward, 25.f, "Wall");

			bool allyDetectedAheadLeft = CollisionManager::Instance().RaycastWithTag(topLeft, up, shortRayLength, "Enemy", gameObject);
			bool allyDetectedAheadRigth = CollisionManager::Instance().RaycastWithTag(topRight, up, shortRayLength, "Enemy", gameObject);

			bool player1DetectedLeft = CollisionManager::Instance().RaycastWithTag(topLeft, up, shortRayLength, "Player1", gameObject);
			bool player1DetectedRight = CollisionManager::Instance().RaycastWithTag(topRight, up, shortRayLength, "Player1", gameObject);

			bool player2DetectedLeft = CollisionManager::Instance().RaycastWithTag(topLeft, up, shortRayLength, "Player1", gameObject);
			bool player2DetectedRight = CollisionManager::Instance().RaycastWithTag(topRight, up, shortRayLength, "Player1", gameObject);

			if (wallAheadTopLeft || wallAheadTopRight || wallAheadCenter || allyDetectedAheadLeft || allyDetectedAheadRigth || player1DetectedLeft || player1DetectedRight)
			{
				DecideTurn(transform, texture, worldRotationDegrees);
			}
			else if (turnCooldown <= 0.f)
			{
				TryRandomTurn(transform, texture, worldRotationDegrees);
			}
		}

		// Renders debug lines to visualize the raycasts used for side checks.
		void Render(SDL_Renderer* renderer) override
		{
			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
			{
				return;
			}

			glm::vec2 center = texture->GetWorldCenter();
			float halfWidth = texture->GetDefaultWidth() / 2.f;
			float halfHeight = texture->GetDefaultHeight() / 2.f;
			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };

			glm::vec2 bottomLeft = center - right * halfWidth + up * halfHeight;
			glm::vec2 topLeft = center - right * halfWidth - up * halfHeight;
			glm::vec2 bottomRight = center + right * halfWidth + up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth - up * halfHeight;

			// Offset the starting point of the side-checking rays slightly forward.
			glm::vec2 forwardOffset = up * offsetDistance;
			float debugRayLength = checkDistance;

			// Debug lines for left side checks.
			//glm::vec2 leftTopStart = topLeft - forwardOffset;
			//DebugDraw::DrawLine(leftTopStart, leftTopStart - right * debugRayLength, { 1, 0, 0, 1 });		// Red
			//glm::vec2 leftBottomStart = bottomLeft + forwardOffset;
			//DebugDraw::DrawLine(leftBottomStart, leftBottomStart - right * debugRayLength, { 0, 1, 0, 1 });	// Green

			//// Debug lines for right side checks.
			//glm::vec2 rightTopStart = topRight - forwardOffset;
			//DebugDraw::DrawLine(rightTopStart, rightTopStart + right * debugRayLength, { 1, 1, 1, 1 });		// White
			//glm::vec2 rightBottomStart = bottomRight + forwardOffset;
			//DebugDraw::DrawLine(rightBottomStart, rightBottomStart + right * debugRayLength, { 0, 1, 1, 1 });	// Cyan
		}

	private:
		float moveSpeed;			   
		float turnCooldown;		   
		float turnCooldownTime;
		float checkDistance;		
		float offsetDistance;		
		std::mt19937 rng;			
		std::uniform_int_distribution<int> flipCoinDistribution; 

		// Rotates a 2D vector by a given angle in degrees.
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

		bool IsSideClear(TextureComponent* texture, const glm::vec2& direction, float checkDistance)
		{
			auto transform = texture->GetOwner()->GetComponent<TransformComponent>();
			if (!transform)
			{
				return false;
			}

			glm::vec2 center = texture->GetWorldCenter();
			float halfWidth = texture->GetDefaultWidth() / 2.f;
			float halfHeight = texture->GetDefaultHeight() / 2.f;
			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };

			glm::vec2 bottomLeft = center - right * halfWidth + up * halfHeight;
			glm::vec2 topLeft = center - right * halfWidth - up * halfHeight;
			glm::vec2 bottomRight = center + right * halfWidth + up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth - up * halfHeight;

			glm::vec2 forwardOffset = up * offsetDistance;
			glm::vec2 normalizedDirection = glm::normalize(direction);

			glm::vec2 startTop, startBottom, startCenter;

			if (glm::dot(normalizedDirection, -right) > 0.9f)
			{
				startTop = topLeft - forwardOffset;
				startBottom = bottomLeft + forwardOffset;
				startCenter = center - right * halfWidth;
				return !CollisionManager::Instance().RaycastWithTag(startTop, normalizedDirection, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startBottom, normalizedDirection, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startCenter, normalizedDirection, checkDistance, "Wall");
			}
			else if (glm::dot(normalizedDirection, right) > 0.9f)
			{
				startTop = topRight - forwardOffset;
				startBottom = bottomRight + forwardOffset;
				startCenter = center + right * halfWidth;
				return !CollisionManager::Instance().RaycastWithTag(startTop, normalizedDirection, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startBottom, normalizedDirection, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startCenter, normalizedDirection, checkDistance, "Wall");
			}
			else
			{
				return false; // Not checking directly left or right.
			}
		}

		void TryRandomTurn(TransformComponent* transform, TextureComponent* texture, float currentRotationDegrees)
		{
			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 leftDirection = RotateVector(forward, 90.f);
			glm::vec2 rightDirection = RotateVector(forward, -90.f);

			bool isLeftClear = IsSideClear(texture, leftDirection, checkDistance);
			bool isRightClear = IsSideClear(texture, rightDirection, checkDistance);

			if (isLeftClear && isRightClear)
			{
				// Randomly choose to turn left or right.
				bool turnLeft = flipCoinDistribution(rng) == 0;
				transform->SetRotation(currentRotationDegrees + (turnLeft ? 90.f : -90.f));
				turnCooldown = turnCooldownTime;
			}
			else if (isLeftClear)
			{
				transform->SetRotation(currentRotationDegrees + 90.f);
				turnCooldown = turnCooldownTime;
			}
			else if (isRightClear)
			{
				transform->SetRotation(currentRotationDegrees - 90.f);
				turnCooldown = turnCooldownTime;
			}
		}

		void DecideTurn(TransformComponent* transform, TextureComponent* texture, float currentRotationDegrees)
		{
			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 leftDirection = RotateVector(forward, 90.f);
			glm::vec2 rightDirection = RotateVector(forward, -90.f);

			bool isLeftClear = IsSideClear(texture, leftDirection, checkDistance);
			bool isRightClear = IsSideClear(texture, rightDirection, checkDistance);

			if (isLeftClear && isRightClear)
			{
				bool turnLeft = flipCoinDistribution(rng) == 0;
				transform->SetRotation(currentRotationDegrees + (turnLeft ? 90.f : -90.f));
				turnCooldown = turnCooldownTime;
			}
			else if (isLeftClear && !isRightClear)
			{
				transform->SetRotation(currentRotationDegrees + 90.f);
			}
			else if (!isLeftClear && isRightClear)
			{
				transform->SetRotation(currentRotationDegrees - 90.f);
			}
			else
			{
				// If neither left nor right is clear, turn around.
				transform->SetRotation(currentRotationDegrees + 180.f);
			}

			turnCooldown = turnCooldownTime;
		}
	};
}