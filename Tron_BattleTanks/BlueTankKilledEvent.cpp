#include "BlueTankKilledEvent.h"
#include "Observer.h"

namespace FML
{
	BlueTankKilledEvent::BlueTankKilledEvent() : scoreForBlueTankKilled(100)
	{
	}
	BlueTankKilledEvent::~BlueTankKilledEvent()
	{
	}
	void BlueTankKilledEvent::Process(Observer& observer) const
	{
		observer.HandleEvent(*this);
	}
}