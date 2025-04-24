#include "GameObjectDestroyedEvent.h"
#include "Observer.h"

namespace FML
{
	void GameObjectDestroyedEvent::Process(Observer& observer) const
	{
		observer.HandleEvent(*this);
	}
}
