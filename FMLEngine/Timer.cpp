#include "Timer.h"

namespace FML
{

	Timer& Timer::Instance() {
		static Timer instance;
		return instance;
	}

	Timer::Timer() : lastTick(SDL_GetTicks()), deltaTime(0), timeSinceStart(0) {}

	void Timer::Start() {
		lastTick = SDL_GetTicks();
		timeSinceStart = 0;
	}

	void Timer::Update() {
		Uint32 currentTick = SDL_GetTicks();
		deltaTime = (currentTick - lastTick) / 1000.0f;
		lastTick = currentTick;
		timeSinceStart += deltaTime;
	}

	float Timer::GetDeltaTime() const {
		return deltaTime;
	}

	float Timer::GetTimeSinceStart() const {
		return timeSinceStart;
	}

	Uint32 Timer::GetLastTick() const {
		return lastTick;
	}


}