#pragma once
#include "Component.h"
#include "RecognizerState.h"
#include <memory>

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
				}

				currentState = std::move(pendingState);
				currentState->Enter(gameObject);
			}
		}

		void ChangeState(std::unique_ptr<RecognizerState> newState)
		{
			pendingState = std::move(newState);
		}

	private:
		std::unique_ptr<RecognizerState> currentState;
		std::unique_ptr<RecognizerState> pendingState;
    };
}
