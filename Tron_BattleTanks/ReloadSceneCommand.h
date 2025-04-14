#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include <iostream>

namespace FML
{
	class ReloadSceneCommand : public Command
	{
	public:
		ReloadSceneCommand() {};

		void Execute() override
		{
			SceneManager::Instance().ReloadScene();
		}

	private:
	};
}
