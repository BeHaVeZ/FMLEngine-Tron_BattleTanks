#include "AgentAvoidance.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "Timer.h"
#include <algorithm>
#include <cmath>
#include <functional>

namespace FML
{
	AgentAvoidance& AgentAvoidance::Instance()
	{
		static AgentAvoidance instance;
		return instance;
	}

	void AgentAvoidance::Register(const void* agent, const glm::vec2& position, float radius, Priority priority)
	{
		if (!agent)
			return;

		const float now = Timer::Instance().GetTimeSinceStart();
		DropStale(now);

		const auto existing = std::find_if(agents.begin(), agents.end(),
			[agent](const Entry& entry) { return entry.agent == agent; });

		Entry& entry = (existing != agents.end()) ? *existing : agents.emplace_back();
		entry.agent = agent;
		entry.position = position;
		entry.radius = radius;
		entry.priority = priority;
		entry.lastSeen = now;
	}

	AgentAvoidance::Verdict AgentAvoidance::Query(const void* agent, const glm::vec2& heading) const
	{
		Verdict verdict;

		const Entry* self = Find(agent);
		if (!self)
			return verdict;

		const float headingLength = glm::length(heading);
		if (headingLength <= 0.f)
			return verdict;

		const glm::vec2 forward = heading / headingLength;
		const glm::vec2 side{ -forward.y, forward.x };

		for (const Entry& other : agents)
		{
			if (other.agent == agent)
				continue;

			const glm::vec2 delta = other.position - self->position;

			const float along = glm::dot(delta, forward);
			if (along <= 0.f)
				continue;

			const float combinedRadius = self->radius + other.radius;

			const float gap = along - combinedRadius;
			if (gap >= lookaheadDistance)
				continue;

			if (std::abs(glm::dot(delta, side)) >= combinedRadius)
				continue;

			const float scale = std::clamp((gap - stopDistance) / (lookaheadDistance - stopDistance), 0.f, 1.f);
			verdict.speedScale = std::min(verdict.speedScale, scale);

			if (scale < stoppedScale && HasRightOfWay(other, *self))
				verdict.giveWay = true;
		}

		return verdict;
	}

	void AgentAvoidance::CollectObstacles(const void* agent, std::vector<glm::vec2>& out) const
	{
		out.clear();

		for (const Entry& entry : agents)
		{
			if (entry.agent != agent)
				out.push_back(entry.position);
		}
	}

	void AgentAvoidance::DebugRenderLane(const void* agent, const glm::vec2& heading) const
	{
		if (!DebugEnabled(DebugChannel::Avoidance))
			return;

		const Entry* self = Find(agent);
		if (!self)
			return;

		DebugDraw::DrawCircle(self->position, self->radius, { .4f, .8f, 1.f, .5f });

		const float headingLength = glm::length(heading);
		if (headingLength <= 0.f)
			return;

		const float scale = Query(agent, heading).speedScale;
		const glm::vec2 forward = heading / headingLength;
		const glm::vec2 laneEnd = self->position + forward * (self->radius + lookaheadDistance);

		DebugDraw::DrawLine(self->position, laneEnd, { 1.f - scale, scale, .2f, .8f });
	}

	void AgentAvoidance::Clear()
	{
		agents.clear();
	}

	const AgentAvoidance::Entry* AgentAvoidance::Find(const void* agent) const
	{
		const auto found = std::find_if(agents.begin(), agents.end(),
			[agent](const Entry& entry) { return entry.agent == agent; });

		return found != agents.end() ? &*found : nullptr;
	}

	void AgentAvoidance::DropStale(float now)
	{
		agents.erase(
			std::remove_if(agents.begin(), agents.end(),
				[now](const Entry& entry) { return now - entry.lastSeen > staleAfter; }),
			agents.end());
	}

	bool AgentAvoidance::HasRightOfWay(const Entry& other, const Entry& self)
	{
		if (other.priority != self.priority)
			return other.priority > self.priority;

		return std::less<const void*>{}(other.agent, self.agent);
	}
}
