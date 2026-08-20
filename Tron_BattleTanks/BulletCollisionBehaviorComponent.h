#pragma once

#include "Component.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "HealthComponent.h"
#include "BulletMoveComponent.h"
#include <string>
#include <cmath>
#include "ServiceLocator.h"
#include "SoundHelper.h"
#include "glm.hpp"
#include "BulletHitEvent.h"
#include "GameTags.h"

namespace FML
{

	struct Separation
	{
		glm::vec2 normal;  
		float     depth;   
	};

	class BulletCollisionBehaviorComponent : public Component
	{
	public:

		void Update(float) override
		{
			resolvedWallHitThisFrame = false;
		}

		void OnTrigger(GameObject* self, Collider* other)
		{
			GameObject* otherGO = other->GetOwner();
			if (!otherGO) return;

			const std::string& tag = otherGO->GetTag();

			if (Tags::IsEnemyTag(tag) || Tags::IsPlayerTag(tag))
			{
				self->GetSubject().Notify(BulletHitEvent(otherGO, self->GetComponent<TransformComponent>()->GetWorldPosition()));

				auto healthComponent = otherGO->GetComponent<HealthComponent>();
				if (healthComponent)
				{
					SoundHelper::PlayRandomSound({ SoundId::Hit1, SoundId::Hit2, SoundId::Hit3 }, .3f);
					healthComponent->Damage(1);
				}
				self->Destroy();
			}
			else if (tag == Tags::Wall)
			{
				ResolveWallHit(self, otherGO);
			}
			else if (tag == Tags::EnemyBullet || tag == Tags::Bullet)
			{
				self->GetSubject().Notify(BulletHitEvent(otherGO, self->GetComponent<TransformComponent>()->GetWorldPosition()));
				self->Destroy();
				otherGO->Destroy();
			}
		}

	private:

		void ResolveWallHit(GameObject* self, GameObject* wall)
		{
			if (resolvedWallHitThisFrame) return;

			auto* bulletCollider = self->GetComponent<BoxCollider>();
			auto* wallCollider = wall->GetComponent<Collider>();
			auto* move = self->GetComponent<BulletMoveComponent>();
			auto* transform = self->GetComponent<TransformComponent>();
			if (!bulletCollider || !wallCollider || !move || !transform) return;

			const glm::vec2 direction = move->GetMoveDirection();

			Separation separation{};
			if (!FindEntryFace(bulletCollider->GetBoundingBox(), wallCollider->GetBoundingBox(), direction, separation))
				return;

			if (glm::dot(direction, separation.normal) >= 0.f)
				return;

			resolvedWallHitThisFrame = true;

			transform->SetPosition(transform->GetLocalPosition() + separation.normal * (separation.depth + separationBias));
			bulletCollider->SyncToTransform();

			self->GetSubject().Notify(BulletHitEvent(wall, transform->GetWorldPosition()));

			move->Bounce(separation.normal);
		}

		static bool FindEntryFace(const SDL_Rect& bullet, const SDL_Rect& wall, const glm::vec2& moveDirection, Separation& outSeparation)
		{
			const float lengthSquared = glm::dot(moveDirection, moveDirection);
			if (lengthSquared < directionEpsilon * directionEpsilon)
				return false;

			const glm::vec2 direction = moveDirection / std::sqrt(lengthSquared);

			const glm::vec2 bulletMin{ static_cast<float>(bullet.x), static_cast<float>(bullet.y) };
			const glm::vec2 bulletMax{ static_cast<float>(bullet.x + bullet.w), static_cast<float>(bullet.y + bullet.h) };
			const glm::vec2 wallMin{ static_cast<float>(wall.x), static_cast<float>(wall.y) };
			const glm::vec2 wallMax{ static_cast<float>(wall.x + wall.w), static_cast<float>(wall.y + wall.h) };

			int   entryAxis = -1;
			float shortestTravel = 0.f;
			float entryDepth = 0.f;

			for (int axis = 0; axis < 2; ++axis)
			{
				if (std::abs(direction[axis]) < directionEpsilon)
					continue;

				const float depth = direction[axis] > 0.f
					? bulletMax[axis] - wallMin[axis]
					: wallMax[axis] - bulletMin[axis];

				if (depth <= 0.f)
					continue;

				const float travel = depth / std::abs(direction[axis]);
				if (entryAxis < 0 || travel < shortestTravel)
				{
					entryAxis = axis;
					shortestTravel = travel;
					entryDepth = depth;
				}
			}

			if (entryAxis < 0)
				return false;

			outSeparation.normal = { 0.f, 0.f };
			outSeparation.normal[entryAxis] = direction[entryAxis] > 0.f ? -1.f : 1.f;
			outSeparation.depth = entryDepth;
			return true;
		}

		bool resolvedWallHitThisFrame = false;

		static constexpr float separationBias = 1.0f;
		static constexpr float directionEpsilon = 1e-4f;
	};
}