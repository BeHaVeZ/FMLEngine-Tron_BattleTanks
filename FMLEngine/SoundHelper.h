#pragma once
#include "ServiceLocator.h"
#include <vector>
#include <random>

namespace FML
{
	class SoundHelper
	{
	public:
		static void PlayRandomSound(const std::vector<SoundId>& soundIds, float volumeBoost = 0.0f)
		{
			if (soundIds.empty()) return;

			static thread_local std::mt19937 rng{ std::random_device{}() };
			std::uniform_int_distribution<size_t> distribution(0, soundIds.size() - 1);
			const SoundId chosenSound = soundIds[distribution(rng)];

			ServiceLocator::GetSoundSystem().PlaySound(chosenSound, ServiceLocator::GetSoundSystem().GetCurrentVolume() + volumeBoost);
		}
		static void LoadSharedSounds()
		{
			auto& soundSystem = ServiceLocator::GetSoundSystem();

			soundSystem.AddSound("bounce_1.wav", SoundId::Bounce1);
			soundSystem.AddSound("bounce_2.wav", SoundId::Bounce2);
			soundSystem.AddSound("bounce_3.wav", SoundId::Bounce3);

			soundSystem.AddSound("hit_1.wav", SoundId::Hit1);
			soundSystem.AddSound("hit_2.wav", SoundId::Hit2);
			soundSystem.AddSound("hit_3.wav", SoundId::Hit3);

			soundSystem.AddSound("Explosion_1.wav", SoundId::Explosion1);
			soundSystem.AddSound("Explosion_2.wav", SoundId::Explosion2);
			soundSystem.AddSound("Explosion_3.wav", SoundId::Explosion3);
			soundSystem.AddSound("Explosion_4.wav", SoundId::Explosion4);

			soundSystem.AddSound("echosplosion_1.wav", SoundId::PlayerExplosion1);
			soundSystem.AddSound("echosplosion_2.wav", SoundId::PlayerExplosion2);

			soundSystem.AddSound("splash.wav", SoundId::Splash);
		}
	};
}
