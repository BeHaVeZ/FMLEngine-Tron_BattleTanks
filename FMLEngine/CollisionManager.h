#pragma once
#include <vector>
#include "Collider.h"
#include <SDL.h>
#include "glm.hpp"
#include <string>
#include <optional>

namespace FML
{
    class CollisionManager 
    {
    public:
        static CollisionManager& Instance() 
        {
            static CollisionManager instance;
            return instance;
        }

        struct RaycastHit 
        {
            GameObject* hitObject;
            glm::vec2 hitPoint;
            float distance;
        };

        CollisionManager(const CollisionManager&) = delete;
        CollisionManager& operator=(const CollisionManager&) = delete;

        void RegisterCollider(Collider* collider);
        void UnregisterCollider(Collider* collider);
        void ClearColliders() { colliders.clear(); }
        void CheckCollisions();

        void ResolveCollision(Collider* colliderA, Collider* colliderB);

        bool Raycast(const glm::vec2& start, const glm::vec2& direction, float maxDistance, GameObject* exclude = nullptr, GameObject* excludeParent = nullptr);
        bool RaycastWithTag(const glm::vec2& start, const glm::vec2& direction, float maxDistance, const std::string& tagToCheck, GameObject* exclude = nullptr, GameObject* excludeParent = nullptr);
        std::optional<RaycastHit> RaycastWithTagHit(const glm::vec2& start, const glm::vec2& direction, float maxDistance, const std::string& tagToCheck, GameObject* exclude = nullptr, GameObject* excludeParent = nullptr);

        bool IntersectRayWithRectangle(const glm::vec2& rayStart, const glm::vec2& rayEnd, const SDL_Rect& rect);


    private:
        CollisionManager() {}
        std::vector<Collider*> colliders;
    };
}
