#pragma once
#include "ServiceLocator.h"
#include <vector>
#include <cstdlib>

namespace FML
{
	class SoundHelper
	{
	public:
		static void PlayRandomSound(const std::vector<SoundId>& soundIds, float volumeBoost = 0.0f)
		{
			if (soundIds.empty()) return;

			int randIndex = rand() % soundIds.size();
			SoundId chosenSound = soundIds[randIndex];

			ServiceLocator::GetSoundSystem().PlaySound(chosenSound, ServiceLocator::GetSoundSystem().GetCurrentVolume() + volumeBoost);
		}
		static void LoadSharedSounds()
		{
			auto& soundSystem = ServiceLocator::GetSoundSystem();

			soundSystem.AddSound("bounce_1.wav", 2);
			soundSystem.AddSound("bounce_2.wav", 3);
			soundSystem.AddSound("bounce_3.wav", 4);

			soundSystem.AddSound("hit_1.wav", 5);
			soundSystem.AddSound("hit_2.wav", 6);
			soundSystem.AddSound("hit_3.wav", 7);

			soundSystem.AddSound("Explosion_1.wav", 10);
			soundSystem.AddSound("Explosion_2.wav", 11);
			soundSystem.AddSound("Explosion_3.wav", 12);
			soundSystem.AddSound("Explosion_4.wav", 13);

			soundSystem.AddSound("splash.wav", 15);
		}
	};
}
