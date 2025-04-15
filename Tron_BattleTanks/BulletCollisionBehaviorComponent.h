#pragma once

#include "Component.h"
#include "Collider.h"
#include "GameObject.h"
#include <string>
#include "ServiceLocator.h"

namespace FML
{
	class BulletCollisionBehaviorComponent : public Component
	{
	public:

		glm::vec2 CalculateCollisionNormal(const SDL_Rect& bulletBox, const SDL_Rect& wallBox)
		{
			int overlapLeft = bulletBox.x + bulletBox.w - wallBox.x;
			int overlapRight = wallBox.x + wallBox.w - bulletBox.x;
			int overlapTop = bulletBox.y + bulletBox.h - wallBox.y;
			int overlapBottom = wallBox.y + wallBox.h - bulletBox.y;

			int minOverlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });

			if (minOverlap == overlapLeft)       return glm::vec2(-1, 0);
			else if (minOverlap == overlapRight) return glm::vec2(1, 0);
			else if (minOverlap == overlapTop)   return glm::vec2(0, -1);
			else                                 return glm::vec2(0, 1);
		}

		void OnCollision(GameObject* self, Collider* other)
		{
			GameObject* otherGO = other->GetOwner();
			if (!otherGO) return;

			const std::string& tag = otherGO->GetTag();

			if (tag == "Enemy" || tag == "Player2" || tag == "Player1")
			{
				auto healthComponent = otherGO->GetComponent<HealthComponent>();
				if (healthComponent)
				{
					ServiceLocator::GetSoundSystem().PlaySound(3, ServiceLocator::GetSoundSystem().GetCurrentVolume() + .3f);
					healthComponent->Damage(1);
				}
				self->Destroy();
			}
			else if (tag == "Wall" || tag == "Bullet")
			{
				auto bulletCollider = self->GetComponent<Collider>();
				auto wallCollider = otherGO->GetComponent<Collider>();

				if (bulletCollider && wallCollider)
				{
					SDL_Rect bulletBox = bulletCollider->GetBoundingBox();
					SDL_Rect wallBox = wallCollider->GetBoundingBox();

					glm::vec2 normal = CalculateCollisionNormal(bulletBox, wallBox);
					self->GetComponent<BulletMoveComponent>()->Bounce(normal);
				}
			}
		}
	};
}