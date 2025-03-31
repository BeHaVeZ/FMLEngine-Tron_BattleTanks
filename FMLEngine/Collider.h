#pragma once
#include "Component.h"
#include <SDL.h>
#include <memory>

namespace FML 
{
    class CollisionManager;

    class Collider : public Component 
    {
    public:
        Collider();
        virtual ~Collider();
        virtual SDL_Rect GetBoundingBox() const = 0;

        bool isStatic;
    };
}