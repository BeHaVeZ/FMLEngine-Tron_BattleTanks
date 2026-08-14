#pragma once
#include "RecognizerState.h"
#include "RecognizerStateComponent.h"
#include "RecognizerMovementHelper.h"
#include "ChasePlayerState.h"
#include "GameObject.h"

namespace FML
{
	class NormalMovingState : public RecognizerState
	{
	public:
		void Update(GameObject* recognizer, float) override
		{
			RecognizerMovementHelper::Move(recognizer, RecognizerMovementHelper::RecognizerSpeed);

			if (RecognizerMovementHelper::PlayerVisible(recognizer))
			{
				recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<ChasePlayerState>());
			}
		}
	};

}