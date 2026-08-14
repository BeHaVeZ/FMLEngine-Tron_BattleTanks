#include "EnemyPerception.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "GameTags.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "CollisionManager.h"
#include <cmath>
#include <optional>

namespace FML
{
	namespace
	{
		GameObject* PlayerFromHit(const std::optional<CollisionManager::RaycastHit>& hit)
		{
			if (!hit || !hit->hitObject)
				return nullptr;

			return Tags::IsPlayerTag(hit->hitObject->GetTag()) ? hit->hitObject : nullptr;
		}
	}

	GameObject* EnemyPerception::SeePlayerAhead(GameObject* agent, float range)
	{
		if (!agent)
			return nullptr;

		const auto* transform = agent->GetComponent<TransformComponent>();
		const auto* texture = agent->GetComponent<TextureComponent>();
		if (!transform || !texture)
			return nullptr;

		const float rotationRadians = glm::radians(transform->GetWorldRotation() + 90.f);
		const glm::vec2 forward{ -std::cos(rotationRadians), -std::sin(rotationRadians) };
		const glm::vec2 right{ -forward.y, forward.x };

		const glm::vec2 center = transform->GetWorldPosition();
		const float halfWidth = texture->GetDefaultWidth() * 0.5f;
		const float halfHeight = texture->GetDefaultHeight() * 0.5f;

		const glm::vec2 frontLeft = center - right * halfWidth + forward * halfHeight;
		const glm::vec2 frontRight = center + right * halfWidth + forward * halfHeight;

		auto& collisions = CollisionManager::Instance();
		const auto leftHit = collisions.RaycastFirstHit(frontLeft, forward, range, agent, nullptr);
		const auto rightHit = collisions.RaycastFirstHit(frontRight, forward, range, agent, nullptr);

		GameObject* seen = PlayerFromHit(leftHit);
		if (!seen)
			seen = PlayerFromHit(rightHit);

		if (DebugEnabled(DebugChannel::Perception) && DebugOverlay::Instance().IsFocused(agent))
		{
			const auto drawSightLine = [&](const glm::vec2& from, const std::optional<CollisionManager::RaycastHit>& hit)
				{
					const glm::vec2 end = hit ? hit->hitPoint : from + forward * range;
					const bool sawPlayer = PlayerFromHit(hit) != nullptr;
					DebugDraw::DrawLine(from, end, sawPlayer ? glm::vec4{ 1.f, .2f, .2f, 1.f } : glm::vec4{ .35f, .55f, 1.f, .55f });
					if (hit)
						DebugDraw::DrawCircle(hit->hitPoint, 3.f, sawPlayer ? glm::vec4{ 1.f, .2f, .2f, 1.f } : glm::vec4{ .35f, .55f, 1.f, .7f });
				};

			drawSightLine(frontLeft, leftHit);
			drawSightLine(frontRight, rightHit);
		}

		return seen;
	}
}
