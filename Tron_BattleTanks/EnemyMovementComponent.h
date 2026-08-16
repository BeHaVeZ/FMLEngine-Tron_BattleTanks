#pragma once
#include "Component.h"
#include "DebugOverlay.h"
#include "EnemyPerception.h"
#include "GameObject.h"
#include "GridMovement.h"
#include "NavGrid.h"
#include "PathFollower.h"
#include "TransformComponent.h"

namespace FML
{
	class EnemyMovementComponent : public Component
	{
	public:
		explicit EnemyMovementComponent(float speed) : movement(speed) {}

		void Update(float deltaTime) override
		{
			if (!NavGrid::Instance().IsBuilt())
				return;

			const glm::vec2 position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();

			GameObject* player = EnemyPerception::SeePlayerAhead(gameObject, sightRange);
			if (player)
			{
				lastKnownPlayerPosition = player->GetComponent<TransformComponent>()->GetWorldPosition();
				chaseTimer = chaseDuration;
			}
			else
			{
				chaseTimer -= deltaTime;
			}

			chasing = chaseTimer > 0.f;

			if (chasing)
			{
				follower.SetGoal(lastKnownPlayerPosition);
			}
			else if (!follower.HasGoal() || follower.ReachedGoal(position) || follower.LastPlanFailed())
			{
				glm::vec2 patrolGoal{};
				if (NavGrid::Instance().FindRandomGoal(position, agentRadius, minPatrolDistance, patrolGoal))
					follower.SetGoal(patrolGoal);
			}

			follower.Update(position, deltaTime);
			movement.FollowPath(gameObject, follower, deltaTime);
		}

		void Render(SDL_Renderer*) override
		{
			auto& overlay = DebugOverlay::Instance();
			const glm::vec2 position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();

			overlay.SubmitFocusCandidate(gameObject, position, gameObject->GetTag());
			if (!overlay.IsFocused(gameObject))
				return;

			NavGrid::Instance().DebugRenderPath(follower.GetPath(), follower.GetNextWaypoint());

			const char* label = chasing ? "CHASE" : (follower.HasPath() ? "PATROL" : "IDLE");

			if (DebugEnabled(DebugChannel::AgentState))
			{
				overlay.WorldText(position + labelOffset, label, { .7f, .85f, 1.f, 1.f });
			}

			overlay.FocusStat(gameObject, std::string("state ") + label);
			overlay.FocusStat(gameObject, "pos   " + std::to_string(static_cast<int>(position.x)) + "," + std::to_string(static_cast<int>(position.y)));
			overlay.FocusStat(gameObject, "path  " + std::to_string(follower.GetNextWaypoint()) + "/" + std::to_string(follower.GetPath().size()));
			overlay.FocusStat(gameObject, "off   " + std::to_string(static_cast<int>(follower.DistanceFromPath(position))) + "px");
		}

	private:
		static constexpr float agentRadius = 16.f;

		GridMovement movement;
		PathFollower follower{ agentRadius };
		glm::vec2 lastKnownPlayerPosition{ 0.f, 0.f };
		float chaseTimer{ 0.f };
		bool chasing{ false };

		static constexpr float sightRange = 600.f;
		static constexpr float chaseDuration = 3.f;
		static constexpr float minPatrolDistance = 160.f;
		static constexpr glm::vec2 labelOffset{ -22.f, -34.f };
	};
}
