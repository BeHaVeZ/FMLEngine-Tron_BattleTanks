#pragma once
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

		const std::vector<glm::vec2>& GetDebugPath() const override { return path; }
		size_t GetDebugNextWaypoint() const override { return nextWaypoint; }
		std::string GetDebugLabel() const override;
		glm::vec2 GetLastKnownPosition() const { return lastKnownPosition; }
		bool HasVisualContact() const { return hasVisualContact; }

	private:
		void Repath(const glm::vec2& from);
		void AdvancePastReachedWaypoints(const glm::vec2& position);

		glm::vec2 lastKnownPosition;
		glm::vec2 plannedFor{ 0.f, 0.f };
		std::vector<glm::vec2> path;
		size_t nextWaypoint{ 0 };
		float searchTimer{ 0.f };
		float repathTimer{ 0.f };
		bool hasVisualContact{ false };

		static constexpr float sightRange = 1000.f;
		static constexpr float searchDuration = 5.f;
		static constexpr float chaseSpeedMultiplier = 1.25f;
		static constexpr float arrivalRadius = 24.f;
		static constexpr float waypointRadius = 12.f;
		static constexpr float agentRadius = 16.f;
		static constexpr float repathInterval = .5f;
		static constexpr float repathDistanceThreshold = 48.f;
	};
}
