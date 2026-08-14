#pragma once
#include "RecognizerState.h"
#include "RecognizerStateComponent.h"


namespace FML
{
    class NormalMovingState;
    class ChasePlayerState : public RecognizerState
    {
    public:
        void Enter(GameObject* recognizer) override;
        void Update(GameObject* recognizer, float dt) override;

	private:
		float chaseTimer{ 0.f };
    };

}