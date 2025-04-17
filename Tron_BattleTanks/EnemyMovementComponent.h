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
			turnCooldownTime(1.2f)
		{
			std::random_device rd;
			rng = std::mt19937(rd());
			flipCoin = std::uniform_int_distribution<int>(0, 1);
		}

		void Update(float deltaTime) override
		{
			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture) return;

			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 position = transform->GetWorldPosition();
			glm::vec2 center = texture->GetWorldCenter();
			float rotation = transform->GetWorldRotation();


			glm::vec2 newPos = position + forward * moveSpeed * deltaTime;
			transform->SetPosition(newPos);

			turnCooldown -= deltaTime;

			bool wallAhead = CollisionManager::Instance().RaycastWithTag(center, forward, 25.f, "Wall");
			bool allyAhaed = CollisionManager::Instance().RaycastWithTag(center, forward, 25.f, "Enemy",this->gameObject);
			bool player1Ahaed = CollisionManager::Instance().RaycastWithTag(center, forward, 25.f, "Player1");
			bool player2Ahaed = CollisionManager::Instance().RaycastWithTag(center, forward, 25.f, "Player2");
			DebugDraw::DrawLine(center, center + forward * 25.f, { 1,1,0,1 });

			if (wallAhead || allyAhaed || player1Ahaed || player2Ahaed)
			{
				DecideTurn(transform, texture, rotation);
			}
			else if (turnCooldown <= 0.f)
			{
				TryRandomTurn(transform, texture, rotation);
			}
		}

		void Render(SDL_Renderer* renderer) override
		{
			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture) return;

			glm::vec2 center = texture->GetWorldCenter();
			float w = texture->GetDefaultWidth() / 2.f;
			float h = texture->GetDefaultHeight() / 2.f;

			float rotation = glm::radians(transform->GetWorldRotation() + 90.f);

			glm::vec2 up = { std::cos(rotation), std::sin(rotation) }; 
			glm::vec2 right = { up.y, -up.x };                       

			glm::vec2 bottomLeft = center - right * w + up * h;
			glm::vec2 topLeft = center - right * w - up * h;
			glm::vec2 bottomRight = center + right * w + up * h;
			glm::vec2 topRight = center + right * w - up * h;

			float debugRayLength = checkDistance;
			glm::vec2 forwardOffset = up * offsetDistance;

			glm::vec2 redStart = topLeft - forwardOffset;
			DebugDraw::DrawLine(redStart, redStart - right * debugRayLength, { 1, 0, 0, 1 }); // RED
			DebugDraw::DrawLine(bottomLeft + forwardOffset, bottomLeft + forwardOffset - right * debugRayLength, { 0,1,0,1 }); //GREEN 

			glm::vec2 whiteStart = topRight - forwardOffset;
			DebugDraw::DrawLine(whiteStart, whiteStart + right * debugRayLength, { 1, 1, 1, 1 }); // WHITE
			DebugDraw::DrawLine(bottomRight + forwardOffset, bottomRight + forwardOffset + right * debugRayLength, { 0,1,1,1 });  //CYAN
		}

	private:
		float moveSpeed;
		float turnCooldown;
		float turnCooldownTime;
		float checkDistance;
		float offsetDistance;
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

		bool IsSideClear(TextureComponent* texture, const glm::vec2& direction, float checkDistance)
		{
			auto transform = texture->GetOwner()->GetComponent<TransformComponent>();
			if (!transform) return false;

			glm::vec2 center = texture->GetWorldCenter();
			float w = texture->GetDefaultWidth() / 2.f;
			float h = texture->GetDefaultHeight() / 2.f;

			float rotation = glm::radians(transform->GetWorldRotation() + 90.f);

			glm::vec2 up = { std::cos(rotation), std::sin(rotation) };
			glm::vec2 right = { up.y, -up.x };

			glm::vec2 bottomLeft = center - right * w + up * h;
			glm::vec2 topLeft = center - right * w - up * h;
			glm::vec2 bottomRight = center + right * w + up * h;
			glm::vec2 topRight = center + right * w - up * h;

			glm::vec2 forwardOffset = up * offsetDistance;
			glm::vec2 dirNorm = glm::normalize(direction);

			glm::vec2 startA, startB, startCenterLeft, startCenterRight;

			if (glm::dot(dirNorm, -right) > 0.9f) // LEFT
			{
				startA = topLeft - forwardOffset;
				startB = bottomLeft + forwardOffset;
				startCenterLeft = center - right * w;
				return !CollisionManager::Instance().RaycastWithTag(startA, dirNorm, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startB, dirNorm, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startCenterLeft, dirNorm, checkDistance, "Wall");
			}
			else if (glm::dot(dirNorm, right) > 0.9f)
			{
				startA = topRight - forwardOffset;
				startB = bottomRight + forwardOffset;
				startCenterRight = center + right * w;
				return !CollisionManager::Instance().RaycastWithTag(startA, dirNorm, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startB, dirNorm, checkDistance, "Wall") &&
					!CollisionManager::Instance().RaycastWithTag(startCenterRight, dirNorm, checkDistance, "Wall");
			}
			else
			{
				return false;
			}
		}


		void TryRandomTurn(TransformComponent* transform, TextureComponent* texture, float rotation)
		{
			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 left = RotateVector(forward, 90.f);
			glm::vec2 right = RotateVector(forward, -90.f);

			bool leftClear = IsSideClear(texture, left, checkDistance);
			bool rightClear = IsSideClear(texture, right, checkDistance);

			if (leftClear && rightClear)
			{
				bool turnLeft = flipCoin(rng) == 0;
				transform->SetRotation(rotation + (turnLeft ? 90.f : -90.f));
				turnCooldown = turnCooldownTime;
			}
			else if (leftClear)
			{
				transform->SetRotation(rotation + 90.f);
				turnCooldown = turnCooldownTime;
			}
			else if (rightClear)
			{
				transform->SetRotation(rotation - 90.f);
				turnCooldown = turnCooldownTime;
			}
		}

		void DecideTurn(TransformComponent* transform, TextureComponent* texture, float rotation)
		{
			glm::vec2 forward = texture->GetForwardVector();
			glm::vec2 left = RotateVector(forward, 90.f);
			glm::vec2 right = RotateVector(forward, -90.f);

			bool leftClear = IsSideClear(texture, left, checkDistance);
			bool rightClear = IsSideClear(texture, right, checkDistance);

			if (leftClear && rightClear)
			{
				bool turnLeft = flipCoin(rng) == 0;
				transform->SetRotation(rotation + (turnLeft ? 90.f : -90.f));
				turnCooldown = turnCooldownTime;
			}
			else if (leftClear && !rightClear)
			{
				transform->SetRotation(rotation + 90.f);
			}
			else if (!leftClear && rightClear)
			{
				transform->SetRotation(rotation - 90.f);
			}
			else
			{
				transform->SetRotation(rotation + 180.f);
			}

			turnCooldown = 1.2f;
		}
	};
}