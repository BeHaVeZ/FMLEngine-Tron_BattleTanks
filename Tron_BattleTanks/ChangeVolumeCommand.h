#pragma once
#include "Command.h"
#include "ConfigManager.h"

namespace FML
{
	class ChangeVolumeCommand : public Command
	{
	public:
		ChangeVolumeCommand(bool toIncrease) : toIncrease(toIncrease) {};

		void Execute() override
		{
			auto& config = ConfigManager::Instance();
			config.SetMasterVolume(config.GetMasterVolume() + (toIncrease ? .1f : -.1f));
			config.ApplyVolume();
		}

	private:
		bool toIncrease;
	};
}
