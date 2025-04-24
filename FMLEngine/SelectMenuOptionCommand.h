#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include "Logger.h"

namespace FML
{
	class SelectMenuOptionCommand : public Command {
	public:
		SelectMenuOptionCommand(GameObject* gameObject) : gameObject(gameObject) {}

		void Execute() override {
			if (gameObject) {
				auto transform = gameObject->GetComponent<TransformComponent>();
				switch ((int)transform->GetLocalRotation())
				{
				case 0:
					SceneManager::Instance().QueueSceneChange("VersusScene");
					break;
				case 90:
					GameStateManager::Instance().SetRunning(false);
					break;
				case 180:
					SceneManager::Instance().QueueSceneChange("CoopScene");
					break;
				case 270:
					SceneManager::Instance().QueueSceneChange("Solo");
					break;

				default:
					Logger::Log(LogLevel::Error, "Invalid rotation value");
					break;
				}
			}
		}

	private:
		GameObject* gameObject;
	};
}
