#include "ChasePlayerState.h"
#include "NormalMovingState.h"
#include "EnemyPerception.h"
#include "RecognizerStateComponent.h"
#include "GridMovement.h"
#include "GameObject.h"
#include "NavGrid.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "TransformComponent.h"
#include <cstdio>

namespace FML
{
	ChasePlayerState::ChasePlayerState(const glm::vec2& lastKnownPlayerPosition)
		: lastKnownPosition(lastKnownPlayerPosition)
	{
	}

	void ChasePlayerState::Enter(GameObject* recognizer, GridMovement& movement)
	{
		searchTimer = searchDuration;
		repathTimer = 0.f;
		movement.SetSpeedMultiplier(chaseSpeedMultiplier);

		Repath(recognizer->GetComponent<TransformComponent>()->GetWorldPosition());
	}

	void ChasePlayerState::Update(GameObject* recognizer, GridMovement& movement, float deltaTime)
	{
		const glm::vec2 position = recognizer->GetComponent<TransformComponent>()->GetWorldPosition();

		GameObject* player = EnemyPerception::SeePlayerAhead(recognizer, sightRange);
		hasVisualContact = player != nullptr;
		if (player)
		{
			lastKnownPosition = player->GetComponent<TransformComponent>()->GetWorldPosition();
			searchTimer = searchDuration;
		}
		else
		{
			searchTimer -= deltaTime;
		}

		repathTimer -= deltaTime;
		const bool targetMoved = glm::distance(lastKnownPosition, plannedFor) > repathDistanceThreshold;
		if (path.empty() || targetMoved || repathTimer <= 0.f)
		{
			Repath(position);
		}

		AdvancePastReachedWaypoints(position);

		const glm::vec2 steeringTarget = (nextWaypoint < path.size()) ? path[nextWaypoint] : lastKnownPosition;
		movement.Seek(recognizer, steeringTarget, deltaTime);

		if (searchTimer <= 0.f)
		{
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<NormalMovingState>());
			return;
		}

		if (!player && glm::distance(position, lastKnownPosition) < arrivalRadius)
		{
			recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<NormalMovingState>());
		}

		if (DebugEnabled(DebugChannel::Perception) && DebugOverlay::Instance().IsFocused(recognizer))
		{
			DebugDraw::DrawCircle(lastKnownPosition, arrivalRadius, { 1.f, .55f, .1f, .9f });
			DebugDraw::DrawLine(position, lastKnownPosition, { 1.f, .55f, .1f, .4f });
		}
	}

	void ChasePlayerState::Exit(GameObject*, GridMovement& movement)
	{
		movement.SetSpeedMultiplier(1.f);
	}

	std::string ChasePlayerState::GetDebugLabel() const
	{
		char buffer[32];
		std::snprintf(buffer, sizeof(buffer), "%s %.1fs", hasVisualContact ? "CHASE" : "SEARCH", searchTimer);
		return buffer;
	}

	void ChasePlayerState::Repath(const glm::vec2& from)
	{
		repathTimer = repathInterval;
		plannedFor = lastKnownPosition;
		nextWaypoint = 0;

		NavGrid::Instance().FindPath(from, lastKnownPosition, agentRadius, path);
	}

	void ChasePlayerState::AdvancePastReachedWaypoints(const glm::vec2& position)
	{
		while (nextWaypoint < path.size() && glm::distance(position, path[nextWaypoint]) < waypointRadius)
		{
			++nextWaypoint;
		}
	}
}
