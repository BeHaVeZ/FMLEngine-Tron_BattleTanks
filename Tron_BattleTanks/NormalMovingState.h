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
		void Update(GameObject* recognizer, float dt) override
		{
			constexpr float recognizerSpeed = 175.f;
			RecognizerMovementHelper::Move(recognizer, recognizerSpeed);

			if (RecognizerMovementHelper::PlayerVisible(recognizer))
			{
				recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(new ChasePlayerState());
			}
		}
	};

}