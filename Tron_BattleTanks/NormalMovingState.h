#pragma once
#include "RecognizerState.h"

namespace FML
{
	class NormalMovingState : public RecognizerState
	{
	public:
		void Update(GameObject* recognizer, GridMovement& movement, float deltaTime) override;
		std::string GetDebugLabel() const override { return "PATROL"; }

	private:
		static constexpr float sightRange = 1000.f;
	};
}
