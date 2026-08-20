#pragma once
#include <SDL.h>

namespace FML
{

	class Timer {
	public:
		static Timer& Instance();
		void Start();
		void Update();
		float GetDeltaTime() const;
		float GetTimeSinceStart() const;
		Uint32 GetLastTick() const;

	private:
		Timer();
		Uint32 lastTick;
		float deltaTime;
		float timeSinceStart;

		static constexpr float maxDeltaTime = 0.033f;
	};

}