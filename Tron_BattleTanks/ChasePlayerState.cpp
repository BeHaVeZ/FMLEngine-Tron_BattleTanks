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

        RecognizerMovementHelper::Move(recognizer, 175.f);

        if (RecognizerMovementHelper::PlayerVisible(recognizer))
        {
            chaseTimer = 5.0f;
        }
        else if (chaseTimer <= 0)
        {
            recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(new NormalMovingState());
        }
	}
    void ChasePlayerState::TurnTowardsPlayer(GameObject*)
    {
    }
}