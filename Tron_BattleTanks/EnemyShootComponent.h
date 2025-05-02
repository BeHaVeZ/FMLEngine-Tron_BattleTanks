#pragma once
#include "Component.h"
#include "PrefabRegistry.h"
#include "CollisionManager.h"
#include "Logger.h"
#include "DebugDraw.h"
#include "SoundHelper.h"

namespace FML
{
	class EnemyShootComponent : public Component
	{
	public:
		explicit EnemyShootComponent() :
			cooldownTime(0.f),
			timeBetweenShots(1.f),
			shootingRange(1000.f)
		{
		};
		~EnemyShootComponent() {};

		void Update(float deltaTime)
		{
			cooldownTime -= deltaTime;

			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
			{
				return;
			}
			float halfWidth = texture->GetDefaultWidth() / 2.f;
			float halfHeight = texture->GetDefaultHeight() / 2.f;

			glm::vec2 center = transform->GetWorldPosition();

			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { -std::cos(rotationRadians), -std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };

			glm::vec2 topLeft = center - right * halfWidth + up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth + up * halfHeight;

			glm::vec2 shootDir = up;

			auto leftHit = CollisionManager::Instance()
				.RaycastFirstHit(topLeft, shootDir, shootingRange, gameObject, nullptr);
			auto rightHit = CollisionManager::Instance()
				.RaycastFirstHit(topRight, shootDir, shootingRange, gameObject, nullptr);

			//DebugDraw::DrawLine(topLeft, topLeft + shootDir * shootingRange, { 1,0,0,1 });
			//DebugDraw::DrawLine(topRight, topRight + shootDir * shootingRange, { 1,0,0,1 });

			if (IsPlayerHit(leftHit) || IsPlayerHit(rightHit))
			{
				Shoot(*texture);
			}

		}

	private:
		void Shoot(TextureComponent& texture)
		{
			if (cooldownTime > 0.f)
				return;
			SoundHelper::PlayRandomSound({ 10,11,12,13 }, .3f);
			SceneManager::Instance().GetCurrentScene()->AddGameObject(PrefabRegistry::Instance().CreateEnemyBulletPrefab(gameObject->GetComponent<TransformComponent>()->GetWorldPosition() + texture.GetForwardVector() * 25.f, texture.GetForwardVector(), "EnemyBullet"));
			cooldownTime = timeBetweenShots;
		}

		bool IsPlayerHit(const std::optional<CollisionManager::RaycastHit>& hit)
		{
			return hit && (hit->hitObject->GetTag() == "Player1" || hit->hitObject->GetTag() == "Player2");
		}

	private:
		float cooldownTime;
		float timeBetweenShots;
		float shootingRange;
	};
}