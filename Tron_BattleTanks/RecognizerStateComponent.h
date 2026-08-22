#pragma once
#include "AgentAvoidance.h"
#include "Component.h"
#include "DebugOverlay.h"
#include "GameObject.h"
#include "GridMovement.h"
#include "NavGrid.h"
#include "RecognizerState.h"
#include "TransformComponent.h"
#include <memory>

namespace FML
{
	class RecognizerStateComponent : public Component
	{
	public:
		static constexpr float RecognizerSpeed = 175.f;

		RecognizerStateComponent() : movement(RecognizerSpeed) {}

		void Update(float deltaTime) override
		{
			if (currentState)
				currentState->Update(gameObject, movement, deltaTime);

			if (pendingState)
			{
				if (currentState)
					currentState->Exit(gameObject, movement);

				currentState = std::move(pendingState);
				currentState->Enter(gameObject, movement);
			}
		}

		void Render(SDL_Renderer*) override
		{
			if (!currentState)
				return;

			auto& overlay = DebugOverlay::Instance();
			if (!overlay.IsMasterEnabled())
				return;

			const glm::vec2 position = gameObject->GetComponent<TransformComponent>()->GetWorldPosition();

			const glm::vec2 heading = currentState->GetDebugHeading(position);
			AgentAvoidance::Instance().DebugRenderLane(gameObject, heading);

			overlay.SubmitFocusCandidate(gameObject, position, gameObject->GetTag());
			if (!overlay.IsFocused(gameObject))
				return;

			NavGrid::Instance().DebugRenderPath(currentState->GetDebugPath(), currentState->GetDebugNextWaypoint());

			if (DebugEnabled(DebugChannel::AgentState))
			{
				overlay.WorldText(position + labelOffset, currentState->GetDebugLabel(), { 1.f, .9f, .3f, 1.f });
			}

			overlay.FocusStat(gameObject, "state " + currentState->GetDebugLabel());
			overlay.FocusStat(gameObject, "pos   " + std::to_string(static_cast<int>(position.x)) + "," + std::to_string(static_cast<int>(position.y)));
			overlay.FocusStat(gameObject, "path  " + std::to_string(currentState->GetDebugNextWaypoint()) + "/" + std::to_string(currentState->GetDebugPath().size()));
			overlay.FocusStat(gameObject, "off   " + std::to_string(static_cast<int>(currentState->GetDebugOffPath(position))) + "px");

			const AgentAvoidance::Verdict traffic = AgentAvoidance::Instance().Query(gameObject, heading);
			overlay.FocusStat(gameObject, "lane  " + std::to_string(static_cast<int>(traffic.speedScale * 100.f)) + "%" + (traffic.giveWay ? " giveway" : ""));
		}

		void ChangeState(std::unique_ptr<RecognizerState> newState)
		{
			pendingState = std::move(newState);
		}

	private:
		GridMovement movement;
		std::unique_ptr<RecognizerState> currentState;
		std::unique_ptr<RecognizerState> pendingState;

		static constexpr glm::vec2 labelOffset{ -20.f, -34.f };
	};
}
