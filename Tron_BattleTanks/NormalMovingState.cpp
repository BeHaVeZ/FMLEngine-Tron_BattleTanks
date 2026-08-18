#include "NormalMovingState.h"
#include "AgentAvoidance.h"
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

		auto& avoidance = AgentAvoidance::Instance();
		avoidance.Register(recognizer, position, agentRadius, AgentAvoidance::Priority::Patrolling);

		if (!follower.HasGoal() || follower.ReachedGoal(position) || follower.LastPlanFailed())
		{
			glm::vec2 patrolGoal{};
			if (NavGrid::Instance().FindRandomGoal(position, agentRadius, minPatrolDistance, patrolGoal))
				follower.SetGoal(patrolGoal);
		}

		follower.Update(recognizer, position, deltaTime);

		const AgentAvoidance::Verdict traffic = avoidance.Query(recognizer, follower.GetHeading(position));
		movement.FollowPath(recognizer, follower, deltaTime, traffic.speedScale);

		giveWayTimer = traffic.giveWay ? giveWayTimer + deltaTime : 0.f;
		if (giveWayTimer >= giveWayPatience)
		{
			giveWayTimer = 0.f;
			follower.Clear();
		}

		if (GameObject* player = EnemyPerception::SeePlayerAhead(recognizer, sightRange))
		{
			const glm::vec2 seenAt = player->GetComponent<TransformComponent>()->GetWorldPosition();
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<ChasePlayerState>(seenAt));
		}
	}
}
