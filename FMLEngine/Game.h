#pragma once
#include <SDL.h>

class Game {
public:
    Game();
    ~Game();

    bool Initialize();

    void Run();

    void ProcessInput();

    void Update(float deltaTime);

    void Render();

    void Cleanup();

    bool IsRunning() const { return isRunning; }

    int GetWindowWidth() const { return windowWidth; }
    int GetWindowHeight() const { return windowHeight; }

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    bool isRunning;

    int windowWidth = 1024;
    int windowHeight = 768;

    int refreshRate = 60;
};
