#pragma once
#include "Component.h"
#include "EnemyPerception.h"
#include "PrefabRegistry.h"
#include "SoundHelper.h"

namespace FML
{
	class EnemyShootComponent : public Component
	{
	public:
		EnemyShootComponent() = default;

		void Update(float deltaTime) override
		{
			cooldownTime -= deltaTime;

			auto* texture = gameObject->GetComponent<TextureComponent>();
			if (!texture)
				return;

			if (EnemyPerception::SeePlayerAhead(gameObject, shootingRange))
			{
				Shoot(*texture);
			}
		}

		void SetBulletSpeed(float speed) { bulletSpeed = speed; }

	private:
		void Shoot(const TextureComponent& texture)
		{
			if (cooldownTime > 0.f)
				return;

			auto* transform = gameObject->GetComponent<TransformComponent>();
			if (!transform)
				return;

			const glm::vec2 forward = texture.GetForwardVector();
			const glm::vec2 origin = transform->GetWorldPosition();
			const glm::vec2 muzzle = origin + forward * bulletSpawnOffset;

			if (EnemyPerception::AllyInLineOfFire(gameObject, muzzle, forward, shootingRange))
				return;

			SoundHelper::PlayRandomSound({ SoundId::Explosion1, SoundId::Explosion2, SoundId::Explosion3, SoundId::Explosion4 }, .3f);

			auto& scene = *SceneManager::Instance().GetCurrentScene();
			scene.AddGameObject(PrefabRegistry::Instance().CreateEnemyBulletPrefab(muzzle, forward, bulletSpeed));

			auto explosion = PrefabRegistry::Instance().CreateTurretShootExplosionPrefab(origin + forward * muzzleFlashOffset);
			explosion->GetComponent<TransformComponent>()->SetRotation(glm::degrees(-std::atan2(forward.y, forward.x)) + muzzleFlashRotationOffset);
			scene.AddGameObject(std::move(explosion));

			cooldownTime = timeBetweenShots;
		}

		float cooldownTime{ 0.f };
		float timeBetweenShots{ 1.f };
		float shootingRange{ 1000.f };
		float bulletSpeed{ 250.f };

		static constexpr float bulletSpawnOffset = 25.f;
		static constexpr float muzzleFlashOffset = 55.f;
		static constexpr float muzzleFlashRotationOffset = 270.f;
	};
}
