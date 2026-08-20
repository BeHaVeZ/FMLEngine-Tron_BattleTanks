#include "ScreenShake.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <random>

namespace FML
{
	ScreenShake& ScreenShake::Instance()
	{
		static ScreenShake instance;
		return instance;
	}

	void ScreenShake::TriggerNpcDeath()
	{
		Trigger(npcTrauma);
	}

	void ScreenShake::TriggerPlayerHit()
	{
		Trigger(playerHitTrauma);
	}

	void ScreenShake::TriggerPlayerDeath()
	{
		Trigger(playerTrauma);
	}

	void ScreenShake::Trigger(float amount)
	{
		static thread_local std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> phase(0.f, 2.f * std::numbers::pi_v<float>);

		phaseX = phase(rng);
		phaseY = phase(rng);

		trauma = std::min(trauma + amount, 1.f);
	}

	void ScreenShake::Update(float deltaTime)
	{
		elapsed += deltaTime;
		trauma = std::max(trauma - traumaDecay * deltaTime, 0.f);
	}

	glm::vec2 ScreenShake::GetOffset() const
	{
		if (trauma <= 0.f)
			return { 0.f, 0.f };

		const float magnitude = maxOffset * trauma * trauma;

		return {
			magnitude * std::sin(elapsed * frequencyX + phaseX),
			magnitude * std::sin(elapsed * frequencyY + phaseY)
		};
	}

	void ScreenShake::Clear()
	{
		trauma = 0.f;
		elapsed = 0.f;
	}
}
