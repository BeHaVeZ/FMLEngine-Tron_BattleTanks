#pragma once
#include <glm.hpp>
#include <vector>

namespace FML
{
	class PathFollower
	{
	public:
		explicit PathFollower(float radius) : agentRadius(radius) {}

		void SetGoal(const glm::vec2& newGoal);
		void Clear();

		void Update(const void* owner, const glm::vec2& position, float deltaTime);

		bool Advance(const glm::vec2& from, float distance, glm::vec2& outPosition, glm::vec2& outHeading);

		glm::vec2 GetHeading(const glm::vec2& from) const;

		bool HasGoal() const { return hasGoal; }
		bool HasPath() const { return nextWaypoint < path.size(); }
		glm::vec2 GetGoal() const { return goal; }

		bool ReachedGoal(const glm::vec2& position) const;

		bool LastPlanFailed() const { return planFailed; }

		const std::vector<glm::vec2>& GetPath() const { return path; }
		size_t GetNextWaypoint() const { return nextWaypoint; }

		float DistanceFromPath(const glm::vec2& position) const;

	private:
		void Repath(const void* owner, const glm::vec2& from);
		bool IsOffPath(const glm::vec2& position) const;

		std::vector<glm::vec2> path;
		std::vector<glm::vec2> obstacles;
		glm::vec2 goal{ 0.f, 0.f };
		glm::vec2 plannedFor{ 0.f, 0.f };
		glm::vec2 pathOrigin{ 0.f, 0.f };
		size_t nextWaypoint{ 0 };
		float agentRadius;
		float repathTimer{ 0.f };
		bool hasGoal{ false };
		bool planFailed{ false };

		static constexpr float repathInterval = .5f;
		static constexpr float goalMovedThreshold = 48.f;
		static constexpr float arrivalRadius = 20.f;

		static constexpr float offPathThreshold = 12.f;
	};
}
