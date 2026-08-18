#include "GridMovement.h"
#include "GameObject.h"
#include "PathFollower.h"
#include "TransformComponent.h"
#include <cmath>

namespace FML
{
	GridMovement::GridMovement(float speed)
		: moveSpeed(speed)
	{
	}

	bool GridMovement::FollowPath(GameObject* agent, PathFollower& follower, float deltaTime, float speedScale)
	{
		auto* transform = agent->GetComponent<TransformComponent>();
		if (!transform)
			return false;

		const glm::vec2 world = transform->GetWorldPosition();

		glm::vec2 nextWorld{};
		glm::vec2 heading{};
		if (!follower.Advance(world, GetSpeed() * speedScale * deltaTime, nextWorld, heading))
			return false;

		transform->SetPosition(transform->GetLocalPosition() + (nextWorld - world));
		FaceDirection(agent, heading);
		return true;
	}

	glm::vec2 GridMovement::DominantAxis(const glm::vec2& direction)
	{
		if (std::abs(direction.x) >= std::abs(direction.y))
			return { direction.x >= 0.f ? 1.f : -1.f, 0.f };

		return { 0.f, direction.y >= 0.f ? 1.f : -1.f };
	}

	void GridMovement::FaceDirection(GameObject* agent, const glm::vec2& direction)
	{
		auto* transform = agent->GetComponent<TransformComponent>();
		if (!transform)
			return;

		const glm::vec2 axis = DominantAxis(direction);
		transform->SetRotation(glm::degrees(std::atan2(-axis.x, -axis.y)));
	}
}
