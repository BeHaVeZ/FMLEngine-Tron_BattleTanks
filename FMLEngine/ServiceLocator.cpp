#include "ServiceLocator.h"

namespace FML
{
	std::unique_ptr<SoundSystem> ServiceLocator::m_pSSInstance{};
	NullSoundSystem ServiceLocator::m_DefaultSS;
}