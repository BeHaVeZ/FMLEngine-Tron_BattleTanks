#pragma once
#include <glm.hpp>
#include <iostream>
#include <SDL.h>
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "DebugDraw.h"
#include "CollisionManager.h"
#include "PrefabRegistry.h"
#include "ServiceLocator.h"

namespace FML
{
	class ShootComponent : public Component
	{
	public:
		ShootComponent(GameObject* owner, float maxRayLength, float shootCooldown)
			: gameObject(owner),
			rayDirection(glm::vec2()),
			maxRayLength(maxRayLength),
			shootCooldown(shootCooldown),
			timeSinceLastShot(0.0f),
			shootAllowanceRange(maxRayLength),
			shootPoint(glm::vec2()),
			canShoot(true)
		{
		}

		void Update(float deltaTime) override
		{
			timeSinceLastShot += deltaTime;
			if (timeSinceLastShot >= shootCooldown && !canShoot) 
			{
				canShoot = true;
			}
		}

		void Render(SDL_Renderer* renderer) override
		{
			if (!renderer) return;

			float angleRadians = glm::radians(gameObject->GetComponent<TransformComponent>()->GetWorldRotation() - 90);

			glm::vec2 pivot = gameObject->GetComponent<TransformComponent>()->GetPivot();
			glm::vec2 pivotPosition = gameObject->GetComponent<TransformComponent>()->GetWorldPosition() + glm::vec2(pivot.x, pivot.y);

			glm::vec2 endPoint = pivotPosition + glm::vec2(cos(angleRadians) * shootAllowanceRange, sin(angleRadians) * shootAllowanceRange);

			glm::vec2 direction = endPoint - pivotPosition;

			shootPoint = endPoint;

			rayDirection = glm::normalize(direction);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawLine(renderer, int(pivotPosition.x), int(pivotPosition.y), int(endPoint.x), int(endPoint.y));
			DebugDraw::DrawCircle(shootPoint, 5.f, { 1.f,0.f,0.f,1.f });
		}

		void Shoot()
		{
			if (!canShoot) return;

			glm::vec2 pivot = gameObject->GetComponent<TransformComponent>()->GetPivot();
			glm::vec2 pivotPosition = gameObject->GetComponent<TransformComponent>()->GetWorldPosition() + glm::vec2(pivot.x, pivot.y);
			GameObject* parent = gameObject->GetParent();

			if (CollisionManager::Instance().Raycast(pivotPosition, rayDirection, maxRayLength, gameObject, parent))
			{
				Logger::Log(LogLevel::Info, "There is something in front of the turret so can't shoot");
			}
			else 
			{
				Logger::Log(LogLevel::Debug, "Shooting from %s", gameObject->GetParent()->GetTag().c_str());
				ServiceLocator::GetSoundSystem().PlaySound(2, ServiceLocator::GetSoundSystem().GetCurrentVolume() + .3f);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(PrefabRegistry::Instance().CreateBulletPrefab({shootPoint},rayDirection,"Bullet"));
				canShoot = false;
				timeSinceLastShot = 0.0f;
			}
		}
		glm::vec2 GetShootPoint() const { return shootPoint; }

	private:
		GameObject* gameObject;
		glm::vec2 rayDirection;
		glm::vec2 shootPoint;
		float maxRayLength;
		float shootCooldown;
		float timeSinceLastShot;
		float shootAllowanceRange;
		bool canShoot;
	};
}
