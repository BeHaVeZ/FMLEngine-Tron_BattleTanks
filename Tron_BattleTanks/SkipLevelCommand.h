#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "SceneManager.h"
#include <iostream>

namespace FML
{
	class SkipLevelCommand : public Command
	{
	public:
		SkipLevelCommand() {};

		void Execute() override
		{
			SceneManager::Instance().GoToNextScene();
		}

	private:
	};
}
