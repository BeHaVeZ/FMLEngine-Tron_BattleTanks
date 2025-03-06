#include "Game.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "TestingScene.h"
#include <SDL_image.h>
#include <iostream>
#include <SDL_ttf.h>
#include "GameStateManager.h"
#include "ConfigManager.h"
#include "SoundSystem.h"
#include "ServiceLocator.h"
#include "MainMenuScene.h"
#include "CoopScene.h"
#include "VersusScene.h"
#include "Timer.h"

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

	SDL_DisplayMode current;
	if (SDL_GetWindowDisplayMode(window, &current) != 0) {	
		std::cerr << "Could not get display mode for video display: " << SDL_GetError() << std::endl;
		refreshRate = 60;
	}
	else {
		refreshRate = current.refresh_rate;
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

	SceneManager::Instance().SetRenderer(renderer);

	auto soundSystem = new SDL_SoundSystem();
	ServiceLocator::RegisterSoundSystem(soundSystem);

	ServiceLocator::GetSoundSystem().StartUp();

	SceneManager::Instance().AddScene("MainMenu", std::make_unique<MainMenuScene>());
	SceneManager::Instance().AddScene("Solo", std::make_unique<TestingScene>());
	SceneManager::Instance().AddScene("CoopScene", std::make_unique<CoopScene>());
	SceneManager::Instance().AddScene("VersusScene", std::make_unique<VersusScene>());

	SceneManager::Instance().ChangeScene("MainMenu");

	GameStateManager::Instance().SetRunning(true);
	return true;
}

void Game::Run() 
{
    const int frameDelay = 1000 / refreshRate;

	Timer::Instance().Start();
    while (GameStateManager::Instance().IsRunning()) {
        Timer::Instance().Update(); 

        float deltaTime = Timer::Instance().GetDeltaTime(); 

        ProcessInput();
        Update(deltaTime);
        Render();

        int frameTime = SDL_GetTicks() - Timer::Instance().GetLastTick();

        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
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
	SceneManager::Instance().Render();
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

	ServiceLocator::GetSoundSystem().Shutdown();

	IMG_Quit();
	SDL_Quit();
}
