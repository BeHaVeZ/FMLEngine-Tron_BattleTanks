#pragma once
#include "Event.h"


namespace FML
{
	class BlueTankKilledEvent : public Event
	{
	public:
		BlueTankKilledEvent();
		~BlueTankKilledEvent();
		void Process(Observer& observer) const override;

		int GetScore() const { return scoreForBlueTankKilled; }
	private:
		int scoreForBlueTankKilled;
	};

}

