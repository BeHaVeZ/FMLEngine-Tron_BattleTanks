#pragma once
#include <glm.hpp>
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "CollisionManager.h"
#include "PrefabRegistry.h"
#include "ServiceLocator.h"
#include "SoundHelper.h"
#include "SceneManager.h"
#include <cmath>

namespace FML
{
	class ShootComponent : public Component
	{
	public:
		ShootComponent(float maxRayLength, float shootCooldown)
			: maxRayLength(maxRayLength),
			shootCooldown(shootCooldown),
			timeSinceLastShot(0.0f),
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

		void Shoot()
		{
			if (!canShoot || !gameObject) return;

			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform) return;

			const float angleRadians = glm::radians(transform->GetWorldRotation() - 90.f);
			const glm::vec2 rayDirection{ std::cos(angleRadians), std::sin(angleRadians) };
			const glm::vec2 pivotPosition = transform->GetWorldPosition() + transform->GetPivot();
			const glm::vec2 shootPoint = pivotPosition + rayDirection * maxRayLength;
			GameObject* parent = gameObject->GetParent();

			if (!CollisionManager::Instance().Raycast(pivotPosition, rayDirection, maxRayLength, gameObject, parent))
			{
				SoundHelper::PlayRandomSound({ SoundId::Explosion1, SoundId::Explosion2, SoundId::Explosion3, SoundId::Explosion4 }, .3f);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(PrefabRegistry::Instance().CreateBulletPrefab({shootPoint},rayDirection,"Bullet"));

				auto explosion = PrefabRegistry::Instance().CreateTurretShootExplosionPrefab(shootPoint);
				float explosionRotation = glm::degrees(-atan2(rayDirection.y, rayDirection.x)) + 270.f;
				explosion->GetComponent<TransformComponent>()->SetRotation(explosionRotation);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(explosion));
				canShoot = false;
				timeSinceLastShot = 0.0f;
			}
		}
	private:
		float maxRayLength;
		float shootCooldown;
		float timeSinceLastShot;
		bool canShoot;
	};
}
