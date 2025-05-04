#pragma once
#include "Component.h"
#include "Collider.h"
#include "HealthComponent.h"
#include "Logger.h"

namespace FML
{
    class RecognizerCollisionBehaviorComponent : public Component
    {
    public:
        void OnCollision(GameObject* self, Collider* other)
        {
            if (!other) return;
            GameObject* otherGO = other->GetOwner();
            if (!otherGO) return;

            const std::string& tag = otherGO->GetTag();

            if (tag == "Player1" || tag == "Player2")
            {
                auto hc = otherGO->GetComponent<HealthComponent>();
                if (hc)
                {
                    Logger::Log(LogLevel::Info, "Recognizer collided with %s, applying damage.", tag.c_str());
                    hc->Damage(1);
                }
            }
        }
    };
}
