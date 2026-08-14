#pragma once
#include <memory>
#include "SoundSystem.h"

namespace FML
{

	class ServiceLocator final
	{
		static std::unique_ptr<SoundSystem> m_pSSInstance;
		static NullSoundSystem m_DefaultSS;

	public:
		static SoundSystem& GetSoundSystem()
		{
			return m_pSSInstance ? *m_pSSInstance : m_DefaultSS;
		}
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem> soundSystem)
		{
			m_pSSInstance = std::move(soundSystem);
		}
	};

}

