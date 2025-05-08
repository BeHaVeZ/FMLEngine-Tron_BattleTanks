#pragma once
#include "Component.h"
#include "RecognizerState.h"

namespace FML
{
    class RecognizerStateComponent : public Component
    {
    public:
        void Update(float dt) override
        {
            if (currentState)
                currentState->Update(gameObject, dt);
        }

        void ChangeState(RecognizerState* newState)
        {
            if (currentState)
            {
                currentState->Exit(gameObject);
                delete currentState;
            }
            currentState = newState;
            if (currentState)
                currentState->Enter(gameObject);
        }

    private:
        RecognizerState* currentState = nullptr;
    };

}