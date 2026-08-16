#pragma once
#include <glm.hpp>

namespace FML
{
	class GameObject;
	class PathFollower;

	class GridMovement
	{
	public:
		explicit GridMovement(float speed);

		bool FollowPath(GameObject* agent, PathFollower& follower, float deltaTime);

		void SetSpeedMultiplier(float multiplier) { speedMultiplier = multiplier; }
		float GetSpeed() const { return moveSpeed * speedMultiplier; }

	private:
		static void FaceDirection(GameObject* agent, const glm::vec2& direction);
		static glm::vec2 DominantAxis(const glm::vec2& direction);

		float moveSpeed;
		float speedMultiplier{ 1.f };
	};
}
