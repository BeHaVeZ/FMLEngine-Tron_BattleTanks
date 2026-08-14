#pragma once

#include "Component.h"
#include "Collider.h"
#include "GameObject.h"
#include <string>
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

	static Separation GetSeparation(const SDL_Rect& a, const SDL_Rect& b)
	{
		int dL = a.x + a.w - b.x;        
		int dR = b.x + b.w - a.x;       
		int dT = a.y + a.h - b.y;        
		int dB = b.y + b.h - a.y;        

		int minPen = std::min({ dL, dR, dT, dB });

		if (minPen == dL) return { {-1, 0}, (float)dL };
		if (minPen == dR) return { { 1, 0}, (float)dR };
		if (minPen == dT) return { { 0,-1}, (float)dT };
		return               { { 0, 1}, (float)dB };
	}

	class BulletCollisionBehaviorComponent : public Component
	{
	public:

		void OnTrigger(GameObject* self, Collider* other)
		{
			GameObject* otherGO = other->GetOwner();
			if (!otherGO) return;

			const std::string& tag = otherGO->GetTag();

			self->GetSubject().Notify(BulletHitEvent(otherGO, self->GetComponent<TransformComponent>()->GetWorldPosition()));
			if (Tags::IsEnemyTag(tag) || Tags::IsPlayerTag(tag))
			{
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
				auto* bulletCol = self->GetComponent<Collider>();
				auto* wallCol = otherGO->GetComponent<Collider>();
				if (!bulletCol || !wallCol) return;

				Separation sep = GetSeparation(bulletCol->GetBoundingBox(),
					wallCol->GetBoundingBox());

				if (auto* move = self->GetComponent<BulletMoveComponent>())
					move->Bounce(sep.normal);

				auto* tr = self->GetComponent<TransformComponent>();
				const float separationBias = 4.0f;
				tr->SetPosition(tr->GetLocalPosition() + sep.normal * (sep.depth + separationBias));
			}
			else if (tag == Tags::EnemyBullet || tag == Tags::Bullet)
			{
				self->Destroy();
				otherGO->Destroy();
			}
		}
	};
}