#pragma once
#include "Event.h"
#include "GameObject.h"

namespace FML
{
	class GameObjectDestroyedEvent : public Event
	{
	public:
		explicit GameObjectDestroyedEvent(GameObject* destroyedObject)
			: destroyed(destroyedObject) {
		}

		GameObject* GetDestroyedObject() const { return destroyed; }

	private:
		GameObject* destroyed;
	};
}
