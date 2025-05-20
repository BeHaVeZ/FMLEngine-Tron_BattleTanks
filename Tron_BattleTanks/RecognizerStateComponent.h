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

            if (pendingState)
            {
                if (currentState)
                {
                    currentState->Exit(gameObject);
                    delete currentState;
                }

                currentState = pendingState;
                currentState->Enter(gameObject);
                pendingState = nullptr;
            }
        }

        void ChangeState(RecognizerState* newState)
        {
            if (pendingState)
            {
                delete pendingState;
            }

            pendingState = newState;
        }

        ~RecognizerStateComponent()
        {
            if (currentState)
                delete currentState;
            if (pendingState)
                delete pendingState;
        }

    private:
        RecognizerState* currentState = nullptr;
        RecognizerState* pendingState = nullptr;
    };
}
