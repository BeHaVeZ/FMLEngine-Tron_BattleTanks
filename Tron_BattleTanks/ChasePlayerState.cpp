#include "ChasePlayerState.h"
#include "NormalMovingState.h"
#include "EnemyPerception.h"
#include "RecognizerStateComponent.h"
#include "GridMovement.h"
#include "GameObject.h"
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

	void ChasePlayerState::Enter(GameObject*, GridMovement& movement)
	{
		searchTimer = searchDuration;
		movement.SetSpeedMultiplier(chaseSpeedMultiplier);

		follower.SetGoal(lastKnownPosition);
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

		follower.SetGoal(lastKnownPosition);
		follower.Update(position, deltaTime);
		movement.FollowPath(recognizer, follower, deltaTime);

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
}
