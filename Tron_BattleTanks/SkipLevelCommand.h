#pragma once
#include "Command.h"
#include "SceneManager.h"

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
