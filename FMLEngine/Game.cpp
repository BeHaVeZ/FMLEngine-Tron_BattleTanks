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
#include "InputHandler.h"
#include "Logger.h"
#include "DebugDraw.h"
#include "../Tron_BattleTanks/SoloScene.h" //This is caused because scenes are being loaded from game but it should not be (GameEngine does not know what scenes any game will have -> abstract

namespace FML
{
	Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

	Game::~Game()
	{
		Cleanup();
	}

	bool Game::Initialize() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			Logger::Log(LogLevel::Error, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
			return false;
		}

		if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		{
			Logger::Log(LogLevel::Error, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
			return false;
		}

		window = SDL_CreateWindow("Tron Battle Tanks - Alexander Terentyev", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ConfigManager::Instance().GetWindowWidth(), ConfigManager::Instance().GetWindowHeight(), SDL_WINDOW_SHOWN);
		if (!window)
		{
			Logger::Log(LogLevel::Error, "Window could not be created! SDL_Error: %s", SDL_GetError());
			return false;
		}

		SDL_DisplayMode current;
		if (SDL_GetWindowDisplayMode(window, &current) != 0)
		{
			Logger::Log(LogLevel::Error, "Could not get display mode for video display: %s", SDL_GetError());
			refreshRate = 60;
		}
		else {
			refreshRate = current.refresh_rate;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (!renderer)
		{
			Logger::Log(LogLevel::Error, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
			return false;
		}

		if (TTF_Init() == -1)
		{
			Logger::Log(LogLevel::Error, "SDL_ttf could not initialize! TTF_Error: %s", TTF_GetError());
			return false;
		}

		SceneManager::Instance().SetRenderer(renderer);

		auto soundSystem = new SDL_SoundSystem();
		ServiceLocator::RegisterSoundSystem(soundSystem);

		ServiceLocator::GetSoundSystem().StartUp();

		SceneManager::Instance().AddScene("MainMenu", std::make_unique<MainMenuScene>());
		SceneManager::Instance().AddScene("Solo", std::make_unique<SoloScene>());
		SceneManager::Instance().AddScene("CoopScene", std::make_unique<CoopScene>());
		SceneManager::Instance().AddScene("VersusScene", std::make_unique<VersusScene>());
		SceneManager::Instance().AddScene("Testing", std::make_unique<TestingScene>());

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
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				GameStateManager::Instance().SetRunning(false);
			}

			if (SceneManager::Instance().GetCurrentScene()) {
				SceneManager::Instance().GetCurrentScene()->HandleInput(event);
			}
		}
	}

	void Game::Update(float deltaTime)
	{
		InputHandler::Instance().Update();
		SceneManager::Instance().Update(deltaTime);
	}

	void Game::Render()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SceneManager::Instance().Render();

		DebugDraw::Render(renderer);

		SDL_RenderPresent(renderer);
	}

	void Game::Cleanup()
	{
		TextureManager::Instance().Clear();

		if (renderer)
		{
			SDL_DestroyRenderer(renderer);
			renderer = nullptr;
		}

		if (window)
		{
			SDL_DestroyWindow(window);
			window = nullptr;
		}

		ServiceLocator::GetSoundSystem().Shutdown();

		IMG_Quit();
		SDL_Quit();
	}
}