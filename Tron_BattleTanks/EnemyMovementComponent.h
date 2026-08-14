#pragma once
#include "Component.h"
#include "DebugOverlay.h"
#include "GameObject.h"
#include "GridMovement.h"
#include "TransformComponent.h"

namespace FML
{
	class EnemyMovementComponent : public Component
	{
	public:
		explicit EnemyMovementComponent(float speed) : movement(speed) {}

		void Update(float deltaTime) override
		{
			movement.Wander(gameObject, deltaTime);
		}

		void Render(SDL_Renderer*) override
		{
			auto& overlay = DebugOverlay::Instance();
			const glm::vec2 position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();

			overlay.SubmitFocusCandidate(gameObject, position, gameObject->GetTag());
			if (!overlay.IsFocused(gameObject))
				return;

			movement.DebugRenderWhiskers();

			if (DebugEnabled(DebugChannel::AgentState))
			{
				overlay.WorldText(position + labelOffset, "WANDER", { .7f, .85f, 1.f, 1.f });
			}

			overlay.FocusStat(gameObject, "state WANDER");
			overlay.FocusStat(gameObject, "pos   " + std::to_string(static_cast<int>(position.x)) + "," + std::to_string(static_cast<int>(position.y)));
		}

	private:
		GridMovement movement;

		static constexpr glm::vec2 labelOffset{ -22.f, -34.f };
	};
}