#include "NormalMovingState.h"
#include "ChasePlayerState.h"
#include "EnemyPerception.h"
#include "RecognizerStateComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"

namespace FML
{
	void NormalMovingState::Update(GameObject* recognizer, GridMovement& movement, float deltaTime)
	{
		movement.Wander(recognizer, deltaTime);

		if (GameObject* player = EnemyPerception::SeePlayerAhead(recognizer, sightRange))
		{
			const glm::vec2 seenAt = player->GetComponent<TransformComponent>()->GetWorldPosition();
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<ChasePlayerState>(seenAt));
		}
	}
}
