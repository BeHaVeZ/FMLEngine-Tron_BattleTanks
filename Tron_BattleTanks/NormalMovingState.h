#pragma once
#include "PathFollower.h"
#include "RecognizerState.h"

namespace FML
{
	class NormalMovingState : public RecognizerState
	{
	public:
		void Update(GameObject* recognizer, GridMovement& movement, float deltaTime) override;

		const std::vector<glm::vec2>& GetDebugPath() const override { return follower.GetPath(); }
		size_t GetDebugNextWaypoint() const override { return follower.GetNextWaypoint(); }
		float GetDebugOffPath(const glm::vec2& position) const override { return follower.DistanceFromPath(position); }
		glm::vec2 GetDebugHeading(const glm::vec2& position) const override { return follower.GetHeading(position); }
		std::string GetDebugLabel() const override { return "PATROL"; }

	private:
		static constexpr float agentRadius = 16.f;

		PathFollower follower{ agentRadius };
		float giveWayTimer{ 0.f };

		static constexpr float sightRange = 1000.f;
		static constexpr float minPatrolDistance = 160.f;
		static constexpr float giveWayPatience = 1.f;
	};
}
