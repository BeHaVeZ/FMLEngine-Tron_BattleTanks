#pragma once
#include "Observer.h"
#include "Component.h"
#include "GameObjectDestroyedEvent.h"
#include "GameAdmin.h"
#include "Logger.h"

namespace FML
{
	class TankObserver : public Component ,public Observer
	{
	public:
		void HandleEvent(const Event& event) override
		{
			auto* destroyEvent = dynamic_cast<const GameObjectDestroyedEvent*>(&event);
			if (!destroyEvent)
				return;

			GameObject* destroyed = destroyEvent->GetDestroyedObject();
			if (destroyed->GetTag() == "Enemy")
			{
				Logger::Log(LogLevel::Error, "Enemy destroyed: Spawning explosion.");
			}
			else 	if (destroyed->GetTag() == "Player1" || destroyed->GetTag() == "Player2")
			{
				GameAdmin::Instance().OnPlayerDestroyed(destroyed);
				Logger::Log(LogLevel::Error, "Player destroyed: Spawning explosion.");
			}
		};
	};
}
