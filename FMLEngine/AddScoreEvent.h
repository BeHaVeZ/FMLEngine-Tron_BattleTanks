#pragma once
#include "Event.h"

class AddScoreEvent final : public Event
{
public:
	AddScoreEvent(int score)
		: Event("EVENT_ADDSCORE")
		, score(score)
	{
	}
	int GetScore() const
	{
		return score;
	}

private:
	int score{};
};

