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
			offsetDistance(5.f),
			turnCooldownTime(2.f),
			minTurnCooldownTime(2.f),
			maxTurnCooldownTime(5.f),
			bottomLeft({}),
			bottomRight({}),
			center({}),
			middleLeft({}),
			middleRight({}),
			topLeft({}),
			topRight({}),
			off({}),
			up({})
		{
			std::random_device rd;
			rng = std::mt19937(rd());
			flipCoinDistribution = std::uniform_int_distribution<int>(0, 1);
		}

		void Update(float dt)
		{
			turnCooldown -= dt;
			CacheFrameData();

			bool frontClear = FrontClear();

			const bool rightClear = RightSideClear();
			const bool leftClear = LeftSideClear();

			if (!frontClear)
			{
				DecideTurn(leftClear, rightClear);
			}
			else if (turnCooldown <= 0 && (leftClear || rightClear))
			{
				DecideTurn(leftClear, rightClear);
				ResetCooldownTimer();
			}
			CacheFrameData();
			MoveUp();
			//DrawDebug();
		}

	private:
		float moveSpeed;
		float turnCooldown;
		float turnCooldownTime;
		float checkDistance;
		float offsetDistance;
		float minTurnCooldownTime;
		float maxTurnCooldownTime;

		glm::vec2 up;
		glm::vec2 right;
		glm::vec2 center;
		glm::vec2 bottomLeft;
		glm::vec2 middleLeft;
		glm::vec2 topLeft;
		glm::vec2 bottomRight;
		glm::vec2 middleRight;
		glm::vec2 topRight;
		glm::vec2 off;

		std::mt19937 rng;
		std::uniform_int_distribution<int> flipCoinDistribution;

		bool FrontClear()
		{
			const bool wallInFront =
				CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, "Wall") ||
				CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, "Wall");

			const bool player1InFront =
				CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, "Player1") ||
				CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, "Player1");

			const bool player2InFront =
				CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, "Player2") ||
				CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, "Player2");

			const bool enemyInFront =
				CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, "Enemy", gameObject) ||
				CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, "Enemy", gameObject);

			if (wallInFront || player1InFront || player2InFront || enemyInFront)
			{
				return false;
			}
			return true;
		}

		void MoveUp()
		{

			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform) return;

			glm::vec2 currentPos = transform->GetLocalPosition();
			glm::vec2 newPos = currentPos + up * moveSpeed * Timer::Instance().GetDeltaTime();
			transform->SetPosition(newPos);
		}

		void CacheFrameData()
		{
			auto* transform = gameObject->GetComponent<TransformComponent>();
			auto* texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
				return;

			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			up = { -std::cos(rotationRadians), -std::sin(rotationRadians) };
			right = { -up.y, up.x };
			center = transform->GetWorldPosition();

			float halfWidth = texture->GetDefaultWidth() * 0.5f;
			float halfHeight = texture->GetDefaultHeight() * 0.5f;

			bottomLeft = center - right * halfWidth - up * halfHeight;
			topLeft = center - right * halfWidth + up * halfHeight;
			bottomRight = center + right * halfWidth - up * halfHeight;
			topRight = center + right * halfWidth + up * halfHeight;

			middleLeft = center - right + up;
			middleRight = center + right + up;

			off = -up * offsetDistance;
		}

		void DrawDebug()
		{
			Logger::Log(LogLevel::Info,
				"Rot=%.0f°  Up=(%.1f,%.1f)  Pos=(%.1f,%.1f)",
				gameObject->GetComponent<TransformComponent>()->GetWorldRotation(),
				up.x, up.y,
				center.x, center.y);

			DebugDraw::DrawLine(center, center + up * checkDistance, { 1,1,1,1 });
			//LEFT
			DebugDraw::DrawLine(topLeft - off, topLeft - off - right * checkDistance, { 1,0,0,1 });
			DebugDraw::DrawLine(bottomLeft + off, bottomLeft + off - right * checkDistance, { 0,0,1,1 });
			DebugDraw::DrawLine(middleLeft, middleLeft - right * checkDistance, { 1,1,1,1 });
			//RIGHT
			DebugDraw::DrawLine(topRight - off, topRight - off + right * checkDistance, { 0,1,0,1 });
			DebugDraw::DrawLine(bottomRight + off, bottomRight + off + right * checkDistance, { 1,0,1,1 });
			DebugDraw::DrawLine(middleRight, middleRight + right * checkDistance, { 1,1,1,1 });
			//FRONT
			DebugDraw::DrawLine(topLeft, topLeft + up * 10.f, { 0, 1, 1, 1 });
			DebugDraw::DrawLine(topRight, topRight + up * 10.f, { 0, 1, 1, 1 });
		}

		void DecideTurn(bool leftClear, bool rightClear)
		{
			int randomSide = flipCoinDistribution(rng); // 0 = LEFT 1 = RIGHT

			if (leftClear && rightClear)
			{
				if (randomSide == 0)
					TurnLeft();
				else
					TurnRight();
			}
			else if (leftClear)
			{
				TurnLeft();
			}
			else if (rightClear)
			{
				TurnRight();
			}
			else
			{
				TurnBack();
			}
		}

		void TurnBack()
		{
			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform) return;
			transform->SetRotation(transform->GetLocalRotation() - 180);
		}
		void TurnRight()
		{
			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform) return;
			transform->SetRotation(transform->GetLocalRotation() - 90);
		}
		void TurnLeft()
		{
			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform) return;
			transform->SetRotation(transform->GetLocalRotation() + 90);
		}

		bool RightSideClear()
		{
			glm::vec2 topRayStart = topRight - off;
			glm::vec2 bottomRayStart = bottomRight + off;
			glm::vec2 middleRayStart = middleRight;

			bool topClear = !CollisionManager::Instance().RaycastWithTag(topRayStart, right, checkDistance, "Wall");
			bool bottomClear = !CollisionManager::Instance().RaycastWithTag(bottomRayStart, right, checkDistance, "Wall");
			bool middleClear = !CollisionManager::Instance().RaycastWithTag(middleRayStart, right, checkDistance, "Wall");

			return topClear && bottomClear && middleClear;
		}

		bool LeftSideClear()
		{
			glm::vec2 topRayStart = topLeft - off;
			glm::vec2 bottomRayStart = bottomLeft + off;
			glm::vec2 middleRayStart = middleLeft;

			bool topClear = !CollisionManager::Instance().RaycastWithTag(topRayStart, -right, checkDistance, "Wall");
			bool bottomClear = !CollisionManager::Instance().RaycastWithTag(bottomRayStart, -right, checkDistance, "Wall");
			bool middleClear = !CollisionManager::Instance().RaycastWithTag(middleRayStart, -right, checkDistance, "Wall");

			return topClear && bottomClear && middleClear;
		}

		void ResetCooldownTimer()
		{
			std::uniform_real_distribution<float> dist(minTurnCooldownTime, maxTurnCooldownTime);
			turnCooldown = dist(rng);
		}
	};
}