#pragma once
#include "Command.h"
#include "ServiceLocator.h"
#include <iostream>

namespace FML
{
	class ChangeVolumeCommand : public Command
	{
	public:
		ChangeVolumeCommand(bool toIncrease) : toIncrease(toIncrease) {};

		void Execute() override
		{
			if (toIncrease)
				ServiceLocator::GetSoundSystem().SetVolume(ServiceLocator::GetSoundSystem().GetCurrentVolume() + .1f);
			else
				ServiceLocator::GetSoundSystem().SetVolume(ServiceLocator::GetSoundSystem().GetCurrentVolume() - .1f);
		}

	private:
		bool toIncrease;
	};
}
