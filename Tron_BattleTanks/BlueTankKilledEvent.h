#pragma once
#include "Event.h"


namespace FML
{
	class BlueTankKilledEvent : public Event
	{
	public:
		BlueTankKilledEvent() = default;

		int GetScore() const { return scoreForBlueTankKilled; }
	private:
		int scoreForBlueTankKilled{ 100 };
	};

}

