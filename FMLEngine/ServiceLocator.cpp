#include "ServiceLocator.h"

std::unique_ptr<SoundSystem> ServiceLocator::m_pSSInstance{};
NullSoundSystem ServiceLocator::m_DefaultSS;