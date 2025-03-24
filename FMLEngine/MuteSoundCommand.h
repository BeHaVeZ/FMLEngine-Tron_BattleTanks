#include "Command.h"
#include "ServiceLocator.h"

namespace FML
{
	class MuteSoundCommand : public Command {
	public:
		void Execute() override {
			auto& soundSystem = ServiceLocator::GetSoundSystem();
			if (soundSystem.isMuted) {
				soundSystem.UnmuteSound();
			}
			else {
				soundSystem.MuteSound();
			}
		}
	};
}


