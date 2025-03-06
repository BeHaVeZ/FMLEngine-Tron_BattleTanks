#pragma once
#include <SDL.h>

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
};
