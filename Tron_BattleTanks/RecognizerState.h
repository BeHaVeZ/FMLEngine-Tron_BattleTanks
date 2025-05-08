#pragma once
#include "GameObject.h"

namespace FML
{
    class RecognizerState
    {
    public:
        virtual ~RecognizerState() = default;

        virtual void Update(GameObject* recognizer, float dt) = 0;

        virtual void Enter(GameObject* recognizer) {};
        virtual void Exit(GameObject* recognizer) {};
    };

}