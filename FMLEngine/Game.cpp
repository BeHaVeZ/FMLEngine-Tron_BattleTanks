#include "Game.h"
#include "TextureManager.h"
#include "SceneManager.h"
//#include "MainMenuScene.h"
//#include "GameplayScene.h"
#include <SDL_image.h>
#include <iostream>
#include <SDL_ttf.h>
#include "GameStateManager.h"
#include "ConfigManager.h"

Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

Game::~Game() {
    Cleanup();
}

bool Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Tron Battle Tanks - Alexander Terentyev", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ConfigManager::Instance().GetWindowWidth(), ConfigManager::Instance().GetWindowHeight(), SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }

    //SceneManager::Instance().AddScene("MainMenu", std::make_unique<MainMenuScene>());
    //SceneManager::Instance().AddScene("Gameplay", std::make_unique<GameplayScene>());

    //SceneManager::Instance().ChangeScene("MainMenu", renderer);

    GameStateManager::Instance().SetRunning(true);
    return true;
}

void Game::Run() {
    Uint32 lastFrameTime = SDL_GetTicks();

    while (GameStateManager::Instance().IsRunning()) {
        Uint32 currentFrameTime = SDL_GetTicks();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;

        ProcessInput();
        Update(deltaTime);
        Render();
    }
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            GameStateManager::Instance().SetRunning(false);
        }

        if (SceneManager::Instance().GetCurrentScene()) {
            SceneManager::Instance().GetCurrentScene()->HandleInput(event);
        }
    }
}

void Game::Update(float deltaTime) {
    SceneManager::Instance().Update(deltaTime);
}

void Game::Render() {
    SDL_RenderClear(renderer);
    SceneManager::Instance().Render(renderer);
    SDL_RenderPresent(renderer);
}

void Game::Cleanup() {
    TextureManager::Instance().Clear();

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}
