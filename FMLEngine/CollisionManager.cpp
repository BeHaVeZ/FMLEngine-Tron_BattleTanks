#include "CollisionManager.h"
#include "CollisionManager.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include <algorithm>
#include "Logger.h"
#include "TransformComponent.h"
#include "BoxCollider.h"
#include "GameObject.h"
#include <cmath>
#include <limits>

namespace FML
{
	void CollisionManager::RegisterCollider(Collider* collider)
	{
		colliders.emplace_back(collider);
	}

	void CollisionManager::UnregisterCollider(Collider* collider)
	{
		if (!colliders.empty())
		{
			colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
		}
	}

	void CollisionManager::DebugRender() const
	{
		if (!DebugEnabled(DebugChannel::Colliders))
			return;

		for (const Collider* collider : colliders)
		{
			if (!collider)
				continue;

			const SDL_Rect box = collider->GetBoundingBox();
			const glm::vec4 color = collider->isTrigger
				? glm::vec4{ 1.f, .9f, .2f, .9f }
				: (collider->isStatic ? glm::vec4{ .55f, .55f, .6f, .7f } : glm::vec4{ .2f, 1.f, .35f, .9f });

			DebugDraw::DrawRectangle(
				{ static_cast<float>(box.x), static_cast<float>(box.y) },
				{ static_cast<float>(box.w), static_cast<float>(box.h) },
				color);
		}
	}

	void CollisionManager::CheckCollisions()
	{
		std::vector<std::pair<Collider*, Collider*>> collisionPairs;

		for (size_t i = 0; i < colliders.size(); ++i)
		{
			for (size_t j = i + 1; j < colliders.size(); ++j)
			{
				Collider* colA = colliders[i];
				Collider* colB = colliders[j];

				if (!colA || !colB) continue;

				SDL_Rect boxA = colA->GetBoundingBox();
				SDL_Rect boxB = colB->GetBoundingBox();

				if (SDL_HasIntersection(&boxA, &boxB))
				{
					GameObject* objA = colA->GetOwner();
					GameObject* objB = colB->GetOwner();

					if (!objA || !objB || objA->IsMarkedForDestruction() || objB->IsMarkedForDestruction())
						continue;

					if (colA->isTrigger || colB->isTrigger)
					{
						if (colA->OnTrigger) colA->OnTrigger(colB);
						if (colB->OnTrigger) colB->OnTrigger(colA);
					}
					else
					{
						collisionPairs.emplace_back(colA, colB);

						if (colA->OnCollision)
							colA->OnCollision(colB);
						if (objB && !objB->IsMarkedForDestruction() && colB->OnCollision)
							colB->OnCollision(colA);
					}
				}
			}
		}

		for (auto& pair : collisionPairs)
		{
			ResolveCollision(pair.first, pair.second);
		}
	}

	bool CollisionManager::SeparateAlongEntryAxis(Collider* a, Collider* b,
		const glm::vec2& centreA, const glm::vec2& centreB,
		const glm::vec2& halfA, const glm::vec2& halfB, glm::vec2& outPush) const
	{
		const bool movingA = !a->isStatic;
		if (movingA == !b->isStatic)
			return false;

		Collider* mover = movingA ? a : b;
		GameObject* owner = mover->GetOwner();
		auto* transform = owner ? owner->GetComponent<TransformComponent>() : nullptr;
		if (!transform)
			return false;

		const glm::vec2 moverCentre = movingA ? centreA : centreB;
		const glm::vec2 blockerCentre = movingA ? centreB : centreA;
		const glm::vec2 combined = halfA + halfB;

		const glm::vec2 entered = moverCentre - blockerCentre
			+ (transform->GetPreviousWorldPosition() - transform->GetWorldPosition());

		const bool clearedX = std::abs(entered.x) >= combined.x;
		const bool clearedY = std::abs(entered.y) >= combined.y;
		if (clearedX == clearedY)
			return false;

		const int axis = clearedX ? 0 : 1;
		const float side = entered[axis] < 0.f ? -1.f : 1.f;

		glm::vec2 push{ 0.f, 0.f };
		push[axis] = blockerCentre[axis] + side * combined[axis] - moverCentre[axis];

		outPush = movingA ? push : -push;
		return true;
	}

	void CollisionManager::ResolveCollision(Collider* a, Collider* b)
	{
		if ((a->isStatic && b->isStatic) || a->isTrigger || b->isTrigger)
			return;

		auto* goA = a->GetOwner();
		auto* goB = b->GetOwner();
		if (!goA || !goB) return;

		auto* tA = goA->GetComponent<TransformComponent>();
		auto* tB = goB->GetComponent<TransformComponent>();
		if (!tA || !tB) return;

		const auto toInfo = [](const SDL_Rect& r)
			{
				glm::vec2 c{ r.x + r.w * 0.5f,  r.y + r.h * 0.5f };
				glm::vec2 h{ r.w * 0.5f,        r.h * 0.5f };
				return std::pair<glm::vec2, glm::vec2>(c, h);
			};

		auto [cA, hA] = toInfo(a->GetBoundingBox());
		auto [cB, hB] = toInfo(b->GetBoundingBox());

		glm::vec2 delta = cA - cB;
		glm::vec2 overlap = hA + hB - glm::abs(delta);
		if (overlap.x <= 0.f || overlap.y <= 0.f) return;

		glm::vec2 push{};
		if (!SeparateAlongEntryAxis(a, b, cA, cB, hA, hB, push))
		{
			if (overlap.x < overlap.y)
				push.x = (delta.x < 0.f ? -overlap.x : overlap.x);
			else
				push.y = (delta.y < 0.f ? -overlap.y : overlap.y);
		}

		if (!a->isStatic)
			tA->SetPosition(tA->GetLocalPosition() + push);

		if (!b->isStatic)
			tB->SetPosition(tB->GetLocalPosition() - push);
	}

	bool CollisionManager::Raycast(const glm::vec2& start, const glm::vec2& direction, float maxDistance, GameObject* exclude, GameObject* excludeParent)
	{
		const glm::vec2 normalizedDirection = glm::normalize(direction);

		for (auto* collider : colliders)
		{
			GameObject* colliderGameObject = collider->GetOwner();
			if (colliderGameObject == exclude || colliderGameObject == excludeParent)
			{
				continue;
			}

			float hitDistance = 0.f;
			if (IntersectRayWithRectangle(start, normalizedDirection, maxDistance, collider->GetBoundingBox(), hitDistance))
			{
				return true;
			}
		}
		return false;
	}

	bool CollisionManager::RaycastWithTag(const glm::vec2& start, const glm::vec2& direction, float maxDistance, std::string_view tagToCheck, GameObject* exclude, GameObject* excludeParent)
	{
		const glm::vec2 normalizedDirection = glm::normalize(direction);

		for (auto* collider : colliders)
		{
			GameObject* colliderGO = collider->GetOwner();
			if (!colliderGO || colliderGO == exclude || colliderGO == excludeParent)
				continue;

			if (colliderGO->GetTag() != tagToCheck)
				continue;

			float hitDistance = 0.f;
			if (IntersectRayWithRectangle(start, normalizedDirection, maxDistance, collider->GetBoundingBox(), hitDistance))
			{
				return true;
			}
		}

		return false;
	}

	std::optional<CollisionManager::RaycastHit> CollisionManager::RaycastFirstHit(
		const glm::vec2& start,
		const glm::vec2& direction,
		float            maxDistance,
		GameObject* exclude,
		GameObject* excludeParent)
	{
		glm::vec2 dirNorm = glm::normalize(direction);
		float     rayLength = std::abs(maxDistance);
		std::optional<RaycastHit> closest;
		float closestDist = rayLength + 1.f;

		for (auto* col : colliders)
		{
			GameObject* go = col->GetOwner();
			if (!go || go == exclude || go == excludeParent)  continue;

			float hitDist;
			if (IntersectRayWithRectangle(start, dirNorm, rayLength, col->GetBoundingBox(), hitDist))
			{
				if (hitDist < closestDist)
				{
					closestDist = hitDist;
					closest = RaycastHit{ go, start + dirNorm * hitDist, hitDist };
				}
			}
		}
		return closest;
	}
	bool CollisionManager::IntersectRayWithRectangle(const glm::vec2& rayStart,
		const glm::vec2& rayDirNorm,
		float            maxDist,
		const SDL_Rect& rect,
		float& outDist)
	{
		float tMin = 0.f;
		float tMax = maxDist;

		if (std::abs(rayDirNorm.x) < std::numeric_limits<float>::epsilon())
		{
			if (rayStart.x < rect.x || rayStart.x > rect.x + rect.w)
				return false;
		}
		else
		{
			float invDx = 1.f / rayDirNorm.x;
			float t1 = (rect.x - rayStart.x) * invDx;
			float t2 = (rect.x + rect.w - rayStart.x) * invDx;
			if (t1 > t2) std::swap(t1, t2);
			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);
			if (tMin > tMax) return false;
		}

		if (std::abs(rayDirNorm.y) < std::numeric_limits<float>::epsilon())
		{
			if (rayStart.y < rect.y || rayStart.y > rect.y + rect.h)
				return false;
		}
		else
		{
			float invDy = 1.f / rayDirNorm.y;
			float t1 = (rect.y - rayStart.y) * invDy;
			float t2 = (rect.y + rect.h - rayStart.y) * invDy;
			if (t1 > t2) std::swap(t1, t2);
			tMin = std::max(tMin, t1);
			tMax = std::min(tMax, t2);
			if (tMin > tMax) return false;
		}

		outDist = tMin;
		return outDist >= 0.f && outDist <= maxDist;
	}

}
