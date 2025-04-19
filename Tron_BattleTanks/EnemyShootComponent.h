#pragma once
#include "Component.h"
#include "PrefabRegistry.h"
#include "CollisionManager.h"
#include "Logger.h"
#include "DebugDraw.h"

namespace FML
{
	class EnemyShootComponent : public Component
	{
	public:
		explicit EnemyShootComponent() :
			cooldownTime(0.f),
			timeBetweenShots(1.f),
			shootingRange(1000.f)
		{};
		~EnemyShootComponent() {};
		void Update(float deltaTime)
		{
			cooldownTime -= deltaTime;
			Logger::Log(LogLevel::Warning, "%d",cooldownTime);
			if (cooldownTime <= 0.f)
			{
				Shoot();
				cooldownTime = timeBetweenShots;
			}

			auto transform = gameObject->GetComponent<TransformComponent>();
			auto texture = gameObject->GetComponent<TextureComponent>();
			if (!transform || !texture)
			{
				return;
			}
			float halfWidth = texture->GetDefaultWidth() / 2.f;
			float halfHeight = texture->GetDefaultHeight() / 2.f;

			glm::vec2 center = texture->GetWorldCenter();

			float worldRotationDegrees = transform->GetWorldRotation();
			float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

			glm::vec2 up = { std::cos(rotationRadians), std::sin(rotationRadians) };
			glm::vec2 right = { up.y, -up.x };

			glm::vec2 topLeft = center - right * halfWidth - up * halfHeight;
			glm::vec2 topRight = center + right * halfWidth - up * halfHeight;

			DebugDraw::DrawLine(topLeft, topLeft + up * -shootingRange, { 1, 0, 0, 1 });
			DebugDraw::DrawLine(topRight, topRight + up * -shootingRange, { 1, 0, 0, 1 });

			bool wallBlocking = CollisionManager::Instance().RaycastWithTag(topLeft, up, -shootingRange, "Wall", gameObject);

			bool playerAhead = //!wallBlocking &&
				(CollisionManager::Instance().RaycastWithTag(topLeft, up, -shootingRange, "Player1", gameObject) ||
					CollisionManager::Instance().RaycastWithTag(topLeft, up, -shootingRange, "Player2", gameObject));


			if (playerAhead)
			{
				Logger::Log(LogLevel::Error, "Player detected");
			}
		}

	private:
		void Shoot()
		{
			PrefabRegistry::Instance().CreateEnemyBulletPrefab(gameObject->GetComponent<TransformComponent>()->GetWorldPosition(), gameObject->GetComponent<TextureComponent>()->GetForwardVector() + 15.f, "EnemyBullet");

		}

	private:
		float cooldownTime;
		float timeBetweenShots;
		float shootingRange;
	};
}