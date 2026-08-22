#pragma once
#include "AgentAvoidance.h"
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

			auto& avoidance = AgentAvoidance::Instance();
			avoidance.Register(gameObject, position, agentRadius,
				chasing ? AgentAvoidance::Priority::Chasing : AgentAvoidance::Priority::Patrolling);

			if (chasing && !follower.LastPlanFailed())
			{
				follower.SetGoal(lastKnownPlayerPosition);
			}
			else if (!follower.HasGoal() || follower.ReachedGoal(position) || follower.LastPlanFailed())
			{
				glm::vec2 patrolGoal{};
				if (NavGrid::Instance().FindRandomGoal(position, agentRadius, minPatrolDistance, patrolGoal))
					follower.SetGoal(patrolGoal);
			}

			follower.Update(gameObject, position, deltaTime);

			const AgentAvoidance::Verdict traffic = avoidance.Query(gameObject, follower.GetHeading(position));
			movement.FollowPath(gameObject, follower, deltaTime, traffic.speedScale);

			ResolveDeadlock(traffic, deltaTime);
		}

		void Render(SDL_Renderer*) override
		{
			auto& overlay = DebugOverlay::Instance();
			if (!overlay.IsMasterEnabled())
				return;

			const glm::vec2 position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();

			AgentAvoidance::Instance().DebugRenderLane(gameObject, follower.GetHeading(position));

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

			const AgentAvoidance::Verdict traffic = AgentAvoidance::Instance().Query(gameObject, follower.GetHeading(position));
			overlay.FocusStat(gameObject, "lane  " + std::to_string(static_cast<int>(traffic.speedScale * 100.f)) + "%" + (traffic.giveWay ? " giveway" : ""));
		}

	private:
		void ResolveDeadlock(const AgentAvoidance::Verdict& traffic, float deltaTime)
		{
			if (!traffic.giveWay)
			{
				giveWayTimer = 0.f;
				return;
			}

			giveWayTimer += deltaTime;
			if (giveWayTimer < giveWayPatience)
				return;

			giveWayTimer = 0.f;

			if (!chasing)
				follower.Clear();
		}

		static constexpr float agentRadius = 16.f;

		GridMovement movement;
		PathFollower follower{ agentRadius };
		glm::vec2 lastKnownPlayerPosition{ 0.f, 0.f };
		float chaseTimer{ 0.f };
		float giveWayTimer{ 0.f };
		bool chasing{ false };

		static constexpr float sightRange = 600.f;
		static constexpr float chaseDuration = 3.f;
		static constexpr float minPatrolDistance = 160.f;
		static constexpr float giveWayPatience = 1.f;
		static constexpr glm::vec2 labelOffset{ -22.f, -34.f };
	};
}
