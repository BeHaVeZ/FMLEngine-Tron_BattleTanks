#pragma once
#include "Component.h"
#include "BlueTankKilledEvent.h"

namespace FML
{
	class ScoreComponent : public Component , public Observer
	{
	public:
		ScoreComponent() : currentScore(0)
		{
		}

		~ScoreComponent()
		{
		}

		void HandleEvent(const Event& event) override
		{
			if (const BlueTankKilledEvent* blueTankKilledEvent = dynamic_cast<const BlueTankKilledEvent*>(&event))
			{
				currentScore += blueTankKilledEvent->GetScore();
				Logger::Log(LogLevel::Info, "ScoreComponent Score updated to %d", currentScore);
			}
		}

		void AddScore()
		{
			gameObject->GetSubject().Notify(BlueTankKilledEvent());
		}


		int GetCurrentScore() const { return currentScore; }

	private:
		int currentScore;
	};

}