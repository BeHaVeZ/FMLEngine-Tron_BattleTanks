#pragma once
#include "InputHandler.h"
#include "MuteSoundCommand.h"
#include "SkipLevelCommand.h"
#include "ReloadSceneCommand.h"

namespace FML
{
	class InputBindingHelper
	{
	public:
		static void BindGlobalCommands();
		static void BindPauseControls();
		static void BindSoloModeControls(GameObject* tank1);
		static void BindDuoModeControls(GameObject* tank1, GameObject* tank2);
	};
}
