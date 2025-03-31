#pragma once
#include <vector>
#include "Collider.h"
#include <SDL.h>

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

        CollisionManager(const CollisionManager&) = delete;
        CollisionManager& operator=(const CollisionManager&) = delete;

        void RegisterCollider(Collider* collider);
        void UnregisterCollider(Collider* collider);
        void CheckCollisions();

        void ResolveCollision(Collider* colliderA, Collider* colliderB);

    private:
        CollisionManager() {}
        std::vector<Collider*> colliders;
    };
}
