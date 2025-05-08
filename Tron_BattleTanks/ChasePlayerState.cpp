#include "ChasePlayerState.h"
#include "NormalMovingState.h"


namespace FML 
{
    void ChasePlayerState::Enter(GameObject* recognizer)
    {
        chaseTimer = 5.0f;
    }

    void ChasePlayerState::Update(GameObject* recognizer, float dt)
	{
        chaseTimer -= dt;

        TurnTowardsPlayer(recognizer);

        if (RecognizerMovementHelper::PlayerVisible(recognizer))
        {
            chaseTimer = 5.0f;
        }
        else if (chaseTimer <= 0)
        {
            recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(new NormalMovingState());
        }
	}
    void ChasePlayerState::TurnTowardsPlayer(GameObject* recognizer)
    {

    }
}