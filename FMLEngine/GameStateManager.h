#pragma once

class GameStateManager {
public:
    static GameStateManager& Instance() {
        static GameStateManager instance;
        return instance;
    }

    bool IsRunning() const { return isRunning; }
    void SetRunning(bool running) { isRunning = running; }

    bool IsPaused() const { return isPaused; }
    void SetPaused(bool paused) { isPaused = paused; }

    GameStateManager(const GameStateManager&) = delete;
    GameStateManager& operator=(const GameStateManager&) = delete;

private:
    GameStateManager() : isRunning(true), isPaused(false) {}

    bool isRunning;
    bool isPaused;
};
