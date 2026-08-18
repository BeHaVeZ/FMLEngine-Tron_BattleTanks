#pragma once
#include <glm.hpp>
#include <vector>

namespace FML
{
	class AgentAvoidance
	{
	public:
		static AgentAvoidance& Instance();

		AgentAvoidance(const AgentAvoidance&) = delete;
		AgentAvoidance& operator=(const AgentAvoidance&) = delete;

		enum class Priority
		{
			Patrolling,
			Chasing
		};

		struct Verdict
		{
			float speedScale{ 1.f };

			bool giveWay{ false };
		};

		void Register(const void* agent, const glm::vec2& position, float radius, Priority priority);

		Verdict Query(const void* agent, const glm::vec2& heading) const;

		void CollectObstacles(const void* agent, std::vector<glm::vec2>& out) const;

		void DebugRenderLane(const void* agent, const glm::vec2& heading) const;

		void Clear();

	private:
		AgentAvoidance() = default;

		struct Entry
		{
			const void* agent{ nullptr };
			glm::vec2 position{ 0.f, 0.f };
			float radius{ 0.f };
			Priority priority{ Priority::Patrolling };
			float lastSeen{ 0.f };
		};

		const Entry* Find(const void* agent) const;
		void DropStale(float now);

		static bool HasRightOfWay(const Entry& other, const Entry& self);

		std::vector<Entry> agents;

		static constexpr float lookaheadDistance = 44.f;
		static constexpr float stopDistance = 6.f;

		static constexpr float stoppedScale = .05f;

		static constexpr float staleAfter = .25f;
	};
}
