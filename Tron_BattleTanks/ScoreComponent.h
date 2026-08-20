#pragma once
#include "Component.h"
#include "BlueTankKilledEvent.h"
#include "GameData.h"

namespace FML
{
	class ScoreComponent : public Component , public Observer
	{
	public:
		ScoreComponent() : currentScore(GameData::CurrentScore)
		{
		}

		~ScoreComponent()
		{
		}

		void HandleEvent(const Event& event) override
		{
			if (const BlueTankKilledEvent* blueTankKilledEvent = dynamic_cast<const BlueTankKilledEvent*>(&event))
			{
				GameData::CurrentScore += blueTankKilledEvent->GetScore();
				currentScore = GameData::CurrentScore;
			}
		}

		void AddScore(int score)
		{
			gameObject->GetSubject().Notify(BlueTankKilledEvent(score));
		}


		int GetCurrentScore() const { return currentScore; }

	private:
		int currentScore;
	};

}