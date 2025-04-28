#include "CollisionManager.h"
#include "CollisionManager.h"
#include <algorithm>
#include "Logger.h"
#include "TransformComponent.h"
#include "BoxCollider.h"
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

	void CollisionManager::ResolveCollision(Collider* a, Collider* b)
	{
		// Skip if both colliders are static or triggers
		if ((a->isStatic && b->isStatic) || a->isTrigger || b->isTrigger)
			return;

		auto* goA = a->GetOwner();
		auto* goB = b->GetOwner();
		if (!goA || !goB) return;

		auto* tA = goA->GetComponent<TransformComponent>();
		auto* tB = goB->GetComponent<TransformComponent>();
		if (!tA || !tB) return;

		// --- convert each rect to centre & half-extents --------------------------
		const auto toInfo = [](const SDL_Rect& r)
			{
				glm::vec2 c{ r.x + r.w * 0.5f,  r.y + r.h * 0.5f };
				glm::vec2 h{ r.w * 0.5f,        r.h * 0.5f };
				return std::pair<glm::vec2, glm::vec2>(c, h);   // { centre, halfSize }
			};

		auto [cA, hA] = toInfo(a->GetBoundingBox());
		auto [cB, hB] = toInfo(b->GetBoundingBox());

		// --- signed overlap ------------------------------------------------------
		glm::vec2 delta = cA - cB;
		glm::vec2 overlap = hA + hB - glm::abs(delta);
		if (overlap.x <= 0.f || overlap.y <= 0.f) return;   // no longer intersecting

		// --- choose axis with the smallest penetration --------------------------
		glm::vec2 push{};
		if (overlap.x < overlap.y)
			push.x = (delta.x < 0.f ? -overlap.x : overlap.x);
		else
			push.y = (delta.y < 0.f ? -overlap.y : overlap.y);

		// --- apply to the non-static collider(s) --------------------------------
		if (!a->isStatic)
			tA->SetPosition(tA->GetLocalPosition() + push);

		if (!b->isStatic)
			tB->SetPosition(tB->GetLocalPosition() - push);
	}

	bool CollisionManager::Raycast(const glm::vec2& start, const glm::vec2& direction, float maxDistance, GameObject* exclude, GameObject* excludeParent)
	{
		bool hitDetected = false;
		glm::vec2 normalizedDirection = glm::normalize(direction);
		glm::vec2 end = start + normalizedDirection * maxDistance;

		for (auto* collider : colliders)
		{
			GameObject* colliderGameObject = collider->GetOwner();
			if (colliderGameObject == exclude || colliderGameObject == excludeParent)
			{
				continue;
			}

			SDL_Rect box = collider->GetBoundingBox();
			if (IntersectRayWithRectangle(start, end, box))
			{
				hitDetected = true;
				//Logger::Log(LogLevel::Debug, "Raycast hit detected with object: %s", colliderGameObject->GetTag().c_str());
			}
		}
		return hitDetected;
	}

	bool CollisionManager::RaycastWithTag(const glm::vec2& start, const glm::vec2& direction, float maxDistance, const std::string& tagToCheck, GameObject* exclude, GameObject* excludeParent)
	{
		bool hitDetected = false;
		glm::vec2 normalizedDirection = glm::normalize(direction);
		glm::vec2 end = start + normalizedDirection * maxDistance;

		for (auto* collider : colliders)
		{
			GameObject* colliderGO = collider->GetOwner();
			if (!colliderGO || colliderGO == exclude || colliderGO == excludeParent)
				continue;

			if (colliderGO->GetTag() != tagToCheck)
				continue;

			SDL_Rect box = collider->GetBoundingBox();
			if (IntersectRayWithRectangle(start, end, box))
			{
				hitDetected = true;
				//Logger::Log(LogLevel::Debug, "RaycastWithTag hit: [%s]", colliderGO->GetTag().c_str());
				break;
			}
		}

		return hitDetected;
	}

	std::optional<CollisionManager::RaycastHit> CollisionManager::RaycastWithTagHit(const glm::vec2& start, const glm::vec2& direction, float maxDistance, const std::string& tagToCheck, GameObject* exclude, GameObject* excludeParent)
	{
		glm::vec2 normalizedDirection = glm::normalize(direction);
		glm::vec2 end = start + normalizedDirection * maxDistance;

		std::optional<RaycastHit> closestHit;
		float closestDistance = std::numeric_limits<float>::max();

		for (auto* collider : colliders)
		{
			GameObject* colliderGO = collider->GetOwner();
			if (!colliderGO || colliderGO == exclude || colliderGO == excludeParent || colliderGO->GetTag() != tagToCheck)
				continue;

			SDL_Rect box = collider->GetBoundingBox();
			if (IntersectRayWithRectangle(start, end, box))
			{
				glm::vec2 hitPoint = start + normalizedDirection * maxDistance;
				float distance = glm::distance(start, hitPoint);

				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestHit = RaycastHit{ colliderGO, hitPoint, distance };
				}
			}
		}

		return closestHit;
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

	bool CollisionManager::IntersectRayWithRectangle(const glm::vec2& rayStart, const glm::vec2& rayEnd, const SDL_Rect& rect)
	{
		float t0 = 0.0f;
		float t1 = std::numeric_limits<float>::max();

		float invDir;
		glm::vec2 rayDir = rayEnd - rayStart;

		if (std::abs(rayDir.x) < std::numeric_limits<float>::epsilon())
		{
			if (rayStart.x < rect.x || rayStart.x >(rect.x + rect.w))
			{
				return false;
			}
		}
		else
		{
			invDir = 1.0f / rayDir.x;
			float tNearX = (rect.x - rayStart.x) * invDir;
			float tFarX = ((rect.x + rect.w) - rayStart.x) * invDir;

			if (tNearX > tFarX) std::swap(tNearX, tFarX);
			t0 = std::max(t0, tNearX);
			t1 = std::min(t1, tFarX);
			if (t0 > t1) return false;
		}
		if (std::abs(rayDir.y) < std::numeric_limits<float>::epsilon())
		{
			if (rayStart.y < rect.y || rayStart.y >(rect.y + rect.h))
			{
				return false;
			}
		}
		else
		{
			invDir = 1.0f / rayDir.y;
			float tNearY = (rect.y - rayStart.y) * invDir;
			float tFarY = ((rect.y + rect.h) - rayStart.y) * invDir;

			if (tNearY > tFarY) std::swap(tNearY, tFarY);
			t0 = std::max(t0, tNearY);
			t1 = std::min(t1, tFarY);
			if (t0 > t1) return false;
		}

		return t1 >= 0 && t0 <= 1.0f;
	}
}