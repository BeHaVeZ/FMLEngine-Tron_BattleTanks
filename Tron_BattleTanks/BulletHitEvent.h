#pragma once
#include "Event.h"
#include "GameObject.h"
#include "glm.hpp"

namespace FML
{
    class BulletHitEvent : public Event
    {
    public:
        BulletHitEvent(GameObject* target, const glm::vec2& hitPosition)
            : target(target), hitPosition(hitPosition)
        {
        }

        void Process(Observer& observer) const override
        {
            observer.HandleEvent(*this);
        };

        GameObject* GetTarget() const { return target; }
        glm::vec2 GetHitPosition() const { return hitPosition; }

    private:
        GameObject* target;
        glm::vec2 hitPosition;
    };
}
