#pragma once
#include "Event.h"


namespace FML
{
	class BlueTankKilledEvent : public Event
	{
	public:
		explicit BlueTankKilledEvent(int score) : scoreForBlueTankKilled(score) {}

		int GetScore() const { return scoreForBlueTankKilled; }
	private:
		int scoreForBlueTankKilled;
	};

}

