#pragma once
#include "PathFollower.h"
#include "RecognizerState.h"
#include <glm.hpp>
#include <vector>

namespace FML
{
	class ChasePlayerState : public RecognizerState
	{
	public:
		explicit ChasePlayerState(const glm::vec2& lastKnownPlayerPosition);

		void Enter(GameObject* recognizer, GridMovement& movement) override;
		void Update(GameObject* recognizer, GridMovement& movement, float deltaTime) override;
		void Exit(GameObject* recognizer, GridMovement& movement) override;

		const std::vector<glm::vec2>& GetDebugPath() const override { return follower.GetPath(); }
		size_t GetDebugNextWaypoint() const override { return follower.GetNextWaypoint(); }
		float GetDebugOffPath(const glm::vec2& position) const override { return follower.DistanceFromPath(position); }
		std::string GetDebugLabel() const override;
		glm::vec2 GetLastKnownPosition() const { return lastKnownPosition; }
		bool HasVisualContact() const { return hasVisualContact; }

	private:
		static constexpr float agentRadius = 16.f;

		glm::vec2 lastKnownPosition;
		PathFollower follower{ agentRadius };
		float searchTimer{ 0.f };
		bool hasVisualContact{ false };

		static constexpr float sightRange = 1000.f;
		static constexpr float searchDuration = 5.f;
		static constexpr float chaseSpeedMultiplier = 1.25f;
		static constexpr float arrivalRadius = 24.f;
	};
}
