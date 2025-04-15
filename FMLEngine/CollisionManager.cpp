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
				SDL_Rect boxA = colliders[i]->GetBoundingBox();
				SDL_Rect boxB = colliders[j]->GetBoundingBox();

				if (SDL_HasIntersection(&boxA, &boxB))
				{
					GameObject* objA = colliders[i]->GetOwner();
					GameObject* objB = colliders[j]->GetOwner();

					if (!objA || !objB || objA->IsMarkedForDestruction() || objB->IsMarkedForDestruction())
						continue;

					collisionPairs.emplace_back(colliders[i], colliders[j]);
					Logger::Log(LogLevel::Debug, "Collision detected between %s and %s at position(%d,%d)", objA->GetTag().c_str(), objB->GetTag().c_str(), boxA.x, boxA.y);

					if (colliders[i]->OnCollision)
						colliders[i]->OnCollision(colliders[j]);

					if (objB && !objB->IsMarkedForDestruction() && colliders[j]->OnCollision)
						colliders[j]->OnCollision(colliders[i]);
				}
			}
		}

		for (auto& pair : collisionPairs)
		{
			ResolveCollision(pair.first, pair.second);
		}
	}

	void CollisionManager::ResolveCollision(Collider* colliderA, Collider* colliderB)
	{
		GameObject* gameObjectA = colliderA->GetOwner();
		GameObject* gameObjectB = colliderB->GetOwner();

		if (!gameObjectA || !gameObjectB)
		{
			Logger::Log(LogLevel::Error, "ResolveCollision: GameObject is null");
			return;
		}

		auto transformA = gameObjectA->GetComponent<TransformComponent>();
		auto transformB = gameObjectB->GetComponent<TransformComponent>();

		if (!transformA || !transformB)
		{
			Logger::Log(LogLevel::Error, "ResolveCollision: TransformComponent is null");
			return;
		}

		SDL_Rect boxA = colliderA->GetBoundingBox();
		SDL_Rect boxB = colliderB->GetBoundingBox();

		int overlapLeft = boxA.x + boxA.w - boxB.x;
		int overlapRight = boxB.x + boxB.w - boxA.x;
		int overlapTop = boxA.y + boxA.h - boxB.y;
		int overlapBottom = boxB.y + boxB.h - boxA.y;

		int minOverlap = std::min({ overlapLeft, overlapRight, overlapTop, overlapBottom });

		if (minOverlap == overlapLeft)
		{
			if (!colliderA->isStatic) {
				transformA->SetPosition({ boxA.x - minOverlap, boxA.y });
			}
			if (!colliderB->isStatic) {
				transformB->SetPosition({ boxB.x + minOverlap, boxB.y });
			}
		}
		else if (minOverlap == overlapRight)
		{
			if (!colliderA->isStatic) {
				transformA->SetPosition({ boxA.x + minOverlap, boxA.y });
			}
			if (!colliderB->isStatic) {
				transformB->SetPosition({ boxB.x - minOverlap, boxB.y });
			}
		}
		else if (minOverlap == overlapTop)
		{
			if (!colliderA->isStatic)
			{
				transformA->SetPosition({ boxA.x, boxA.y - minOverlap });
			}
			if (!colliderB->isStatic)
			{
				transformB->SetPosition({ boxB.x, boxB.y + minOverlap });
			}
		}
		else {
			if (!colliderA->isStatic) {
				transformA->SetPosition({ boxA.x, boxA.y + minOverlap });
			}
			if (!colliderB->isStatic) {
				transformB->SetPosition({ boxB.x, boxB.y - minOverlap });
			}
		}
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
				Logger::Log(LogLevel::Debug, "Raycast hit detected with object: %s", colliderGameObject->GetTag().c_str());
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
				Logger::Log(LogLevel::Debug, "RaycastWithTag hit: [%s]", colliderGO->GetTag().c_str());
				break;
			}
		}

		return hitDetected;
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