#include "ChasePlayerState.h"
#include "NormalMovingState.h"


namespace FML 
{
    void ChasePlayerState::Enter(GameObject*)
    {
        chaseTimer = 5.0f;
    }

    void ChasePlayerState::Update(GameObject* recognizer, float dt)
	{
        chaseTimer -= dt;

		RecognizerMovementHelper::Move(recognizer, RecognizerMovementHelper::RecognizerSpeed);

        if (RecognizerMovementHelper::PlayerVisible(recognizer))
        {
            chaseTimer = 5.0f;
        }
        else if (chaseTimer <= 0)
        {
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<NormalMovingState>());
		}
	}
}