#include "PathFollower.h"
#include "AgentAvoidance.h"
#include "NavGrid.h"
#include <algorithm>

namespace FML
{
	namespace
	{
		constexpr float waypointEpsilon = .001f;

		float DistanceToSegment(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end)
		{
			const glm::vec2 segment = end - start;
			const float lengthSquared = glm::dot(segment, segment);
			if (lengthSquared <= 0.f)
				return glm::distance(point, start);

			const float t = std::clamp(glm::dot(point - start, segment) / lengthSquared, 0.f, 1.f);
			return glm::distance(point, start + segment * t);
		}
	}

	void PathFollower::SetGoal(const glm::vec2& newGoal)
	{
		const bool changed = !hasGoal || glm::distance(newGoal, goal) > 1.f;
		goal = newGoal;

		if (changed)
			planFailed = false;

		if (hasGoal)
			return;

		hasGoal = true;
		repathTimer = 0.f;
		nextWaypoint = 0;
		path.clear();
	}

	void PathFollower::Clear()
	{
		path.clear();
		nextWaypoint = 0;
		repathTimer = 0.f;
		hasGoal = false;
		planFailed = false;
	}

	void PathFollower::Update(const void* owner, const glm::vec2& position, float deltaTime)
	{
		if (!hasGoal)
			return;

		repathTimer -= deltaTime;

		const bool goalMoved = glm::distance(goal, plannedFor) > goalMovedThreshold;
		const bool pathExhausted = nextWaypoint >= path.size();
		const bool needsPlan = pathExhausted || goalMoved || IsOffPath(position);

		if (needsPlan && repathTimer <= 0.f && !ReachedGoal(position))
		{
			Repath(owner, position);
		}
	}

	bool PathFollower::Advance(const glm::vec2& from, float distance, glm::vec2& outPosition, glm::vec2& outHeading)
	{
		outPosition = from;
		outHeading = glm::vec2{ 0.f, 0.f };

		if (distance <= 0.f)
			return false;

		bool moved = false;
		float remaining = distance;

		while (remaining > 0.f && nextWaypoint < path.size())
		{
			const glm::vec2 toWaypoint = path[nextWaypoint] - outPosition;
			const float length = glm::length(toWaypoint);

			if (length <= waypointEpsilon)
			{
				++nextWaypoint;
				continue;
			}

			outHeading = toWaypoint / length;
			moved = true;

			if (length > remaining)
			{
				outPosition += outHeading * remaining;
				break;
			}

			outPosition = path[nextWaypoint];
			remaining -= length;
			++nextWaypoint;
		}

		return moved;
	}

	glm::vec2 PathFollower::GetHeading(const glm::vec2& from) const
	{
		if (nextWaypoint >= path.size())
			return { 0.f, 0.f };

		const glm::vec2 toWaypoint = path[nextWaypoint] - from;
		const float length = glm::length(toWaypoint);

		return length > waypointEpsilon ? toWaypoint / length : glm::vec2{ 0.f, 0.f };
	}

	bool PathFollower::ReachedGoal(const glm::vec2& position) const
	{
		return hasGoal && glm::distance(position, goal) < arrivalRadius;
	}

	float PathFollower::DistanceFromPath(const glm::vec2& position) const
	{
		if (nextWaypoint >= path.size())
			return 0.f;

		const glm::vec2 legStart = (nextWaypoint == 0) ? pathOrigin : path[nextWaypoint - 1];
		return DistanceToSegment(position, legStart, path[nextWaypoint]);
	}

	bool PathFollower::IsOffPath(const glm::vec2& position) const
	{
		return DistanceFromPath(position) > offPathThreshold;
	}

	void PathFollower::Repath(const void* owner, const glm::vec2& from)
	{
		repathTimer = repathInterval;
		plannedFor = goal;
		pathOrigin = from;
		nextWaypoint = 0;

		AgentAvoidance::Instance().CollectObstacles(owner, obstacles);

		NavGrid::Instance().FindPath(from, goal, agentRadius, path, obstacles);
		planFailed = path.empty();
	}
}
