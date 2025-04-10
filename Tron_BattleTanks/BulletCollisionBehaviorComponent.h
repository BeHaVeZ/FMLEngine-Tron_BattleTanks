#pragma once

#include "Component.h"
#include "Collider.h"
#include "GameObject.h"
#include <string>

namespace FML
{
    class BulletCollisionBehaviorComponent : public Component
    {
    public:
        void OnCollision(GameObject* self, Collider* other)
        {
            GameObject* otherGO = other->GetOwner();
            if (!otherGO) return;

            const std::string& tag = otherGO->GetTag();

            if (tag == "Enemy" || tag == "Player2") 
            {
                self->Destroy();
            }
            else if (tag == "Wall") 
            {
                // todo: Bounce off the wall
            }
        }
    };
}