#pragma once
#include <glm.hpp>

namespace FML
{
	class ScreenShake
	{
	public:
		static ScreenShake& Instance();

		ScreenShake(const ScreenShake&) = delete;
		ScreenShake& operator=(const ScreenShake&) = delete;

		void TriggerNpcDeath();
		void TriggerPlayerHit();
		void TriggerPlayerDeath();

		void Update(float deltaTime);

		glm::vec2 GetOffset() const;

		void Clear();

	private:
		ScreenShake() = default;

		void Trigger(float amount);

		float trauma{ 0.f };
		float elapsed{ 0.f };
		float phaseX{ 0.f };
		float phaseY{ 0.f };

		static constexpr float maxOffset = 12.f;

		static constexpr float traumaDecay = 2.2f;

		static constexpr float frequencyX = 41.f;
		static constexpr float frequencyY = 37.f;

		static constexpr float npcTrauma = .55f;
		static constexpr float playerHitTrauma = .75f;
		static constexpr float playerTrauma = 1.f;
	};
}
