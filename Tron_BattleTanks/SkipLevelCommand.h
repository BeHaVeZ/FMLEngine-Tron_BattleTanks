#pragma once
#include "Command.h"
#include "SceneManager.h"
#include "GameAdmin.h"

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
