#include "CollisionManager.h"
#include <algorithm>
#include "Logger.h"
#include "TransformComponent.h"
#include "BoxCollider.h"

namespace FML
{
    void CollisionManager::RegisterCollider(Collider* collider) {
        colliders.emplace_back(collider);
    }

    void CollisionManager::UnregisterCollider(Collider* collider) {
        colliders.erase(std::remove(colliders.begin(), colliders.end(), collider), colliders.end());
    }

    void CollisionManager::CheckCollisions() {
        std::vector<std::pair<Collider*, Collider*>> collisionPairs;

        for (size_t i = 0; i < colliders.size(); ++i) 
        {
            for (size_t j = i + 1; j < colliders.size(); ++j) 
            {
                SDL_Rect boxA = colliders[i]->GetBoundingBox();
                SDL_Rect boxB = colliders[j]->GetBoundingBox();

                if (SDL_HasIntersection(&boxA, &boxB)) 
                {
                    collisionPairs.emplace_back(colliders[i], colliders[j]);
                    Logger::Log(LogLevel::Warning, "Collision Detected");
                }
            }
        }

        for (auto& pair : collisionPairs) 
        {
            ResolveCollision(pair.first, pair.second);
        }
    }

    void CollisionManager::ResolveCollision(Collider* colliderA, Collider* colliderB) {
        GameObject* gameObjectA = colliderA->GetOwner();
        GameObject* gameObjectB = colliderB->GetOwner();

        if (!gameObjectA || !gameObjectB) {
            Logger::Log(LogLevel::Error, "ResolveCollision: GameObject is null");
            return;
        }

        auto transformA = gameObjectA->GetComponent<TransformComponent>();
        auto transformB = gameObjectB->GetComponent<TransformComponent>();

        if (!transformA || !transformB) {
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

        colliderA->GetOwner()->HandleCollisionEvent(colliderB->GetOwner());
        colliderB->GetOwner()->HandleCollisionEvent(colliderA->GetOwner());
    }
}