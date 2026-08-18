#pragma once
#include <glm.hpp>
#include <string>
#include <vector>

namespace FML
{
	class GameObject;
	class GridMovement;

	class RecognizerState
	{
	public:
		virtual ~RecognizerState() = default;

		virtual void Update(GameObject* recognizer, GridMovement& movement, float deltaTime) = 0;

		virtual void Enter(GameObject*, GridMovement&) {};
		virtual void Exit(GameObject*, GridMovement&) {};

		virtual const std::vector<glm::vec2>& GetDebugPath() const
		{
			static const std::vector<glm::vec2> none;
			return none;
		}
		virtual size_t GetDebugNextWaypoint() const { return 0; }

		virtual float GetDebugOffPath(const glm::vec2&) const { return 0.f; }

		virtual glm::vec2 GetDebugHeading(const glm::vec2&) const { return { 0.f, 0.f }; }

		virtual std::string GetDebugLabel() const = 0;
	};
}
