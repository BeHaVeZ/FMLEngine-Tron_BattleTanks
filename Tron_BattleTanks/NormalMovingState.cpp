#include "NormalMovingState.h"
#include "ChasePlayerState.h"
#include "EnemyPerception.h"
#include "RecognizerStateComponent.h"
#include "GridMovement.h"
#include "GameObject.h"
#include "NavGrid.h"
#include "TransformComponent.h"

namespace FML
{
	void NormalMovingState::Update(GameObject* recognizer, GridMovement& movement, float deltaTime)
	{
		const glm::vec2 position = recognizer->GetComponent<TransformComponent>()->GetWorldPosition();

		if (!follower.HasGoal() || follower.ReachedGoal(position) || follower.LastPlanFailed())
		{
			glm::vec2 patrolGoal{};
			if (NavGrid::Instance().FindRandomGoal(position, agentRadius, minPatrolDistance, patrolGoal))
				follower.SetGoal(patrolGoal);
		}

		follower.Update(position, deltaTime);
		movement.FollowPath(recognizer, follower, deltaTime);

		if (GameObject* player = EnemyPerception::SeePlayerAhead(recognizer, sightRange))
		{
			const glm::vec2 seenAt = player->GetComponent<TransformComponent>()->GetWorldPosition();
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<ChasePlayerState>(seenAt));
		}
	}
}
