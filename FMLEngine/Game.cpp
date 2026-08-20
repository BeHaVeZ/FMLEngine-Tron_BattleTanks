#include "Game.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "TestingScene.h"
#include <SDL_image.h>
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
#include "DebugOverlay.h"
#include "ScreenShake.h"
#include "PauseMenu.h"
#include "../Tron_BattleTanks/SoloScene.h" //This is caused because scenes are being loaded from game but it should not be (GameEngine does not know what scenes any game will have -> abstract
#include "../Tron_BattleTanks/SoloLevel2.h"
#include "../Tron_BattleTanks/SoloLevel3.h"
#include "../Tron_BattleTanks/SoloHighscoreScene.h"
#include "../Tron_BattleTanks/NameEntryScene.h"
#include "../Tron_BattleTanks/VersusResultScene.h"
#include "CollisionManager.h"
#include "../Tron_BattleTanks/Level404.h"
#include <filesystem>

namespace
{
	bool SetWorkingDirectoryToExecutable()
	{
		char* executableDirectory = SDL_GetBasePath();
		if (!executableDirectory)
		{
			FML::Logger::Log(FML::LogLevel::Error, "Could not locate the executable directory: %s", SDL_GetError());
			return false;
		}

		std::error_code error;
		std::filesystem::current_path(executableDirectory, error);
		SDL_free(executableDirectory);

		if (error)
		{
			FML::Logger::Log(FML::LogLevel::Error, "Could not use the executable directory as the working directory: %s", error.message().c_str());
			return false;
		}

		return true;
	}
}

namespace FML
{
	Game::Game() : window(nullptr), renderer(nullptr), isRunning(false) {}

	Game::~Game()
	{
		Cleanup();
	}

	bool Game::Initialize() 
	{
		std::srand(static_cast<unsigned>(std::time(nullptr)));
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			Logger::Log(LogLevel::Error, "SDL could not initialize! SDL_Error: %s", SDL_GetError());
			return false;
		}

		if (!SetWorkingDirectoryToExecutable())
		{
			return false;
		}

		ConfigManager::Instance().Load();

		if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		{
			Logger::Log(LogLevel::Error, "SDL_image could not initialize! SDL_image Error: %s", IMG_GetError());
			return false;
		}

		Uint32 windowFlags = SDL_WINDOW_SHOWN;
		if (ConfigManager::Instance().IsFullscreen())
		{
			windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow("Tron Battle Tanks - Alexander Terentyev", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ConfigManager::Instance().GetDisplayWidth(), ConfigManager::Instance().GetDisplayHeight(), windowFlags);
		if (!window)
		{
			Logger::Log(LogLevel::Error, "Window could not be created! SDL_Error: %s", SDL_GetError());
			return false;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (!renderer)
		{
			Logger::Log(LogLevel::Error, "Renderer could not be created! SDL_Error: %s", SDL_GetError());
			return false;
		}

		ConfigManager::Instance().AttachWindow(window, renderer);
		ConfigManager::Instance().ApplyScaling();

		if (TTF_Init() == -1)
		{
			Logger::Log(LogLevel::Error, "SDL_ttf could not initialize! TTF_Error: %s", TTF_GetError());
			return false;
		}

		SceneManager::Instance().SetRenderer(renderer);
		DebugOverlay::Instance().Initialize(renderer, "data/fonts/tron-arcade.ttf", 12);

		ServiceLocator::RegisterSoundSystem(std::make_unique<SDL_SoundSystem>());

		ServiceLocator::GetSoundSystem().StartUp();
		ConfigManager::Instance().ApplyVolume();

		SceneManager::Instance().AddScene(std::make_unique<MainMenuScene>());
		SceneManager::Instance().AddScene(std::make_unique<SoloScene>());
		SceneManager::Instance().AddScene(std::make_unique<SoloLevel2>());
		SceneManager::Instance().AddScene(std::make_unique<SoloLevel3>());
		SceneManager::Instance().AddScene(std::make_unique<Level404>());
		SceneManager::Instance().AddScene(std::make_unique<VersusScene>());
		SceneManager::Instance().AddScene(std::make_unique<CoopScene>());
		SceneManager::Instance().AddScene(std::make_unique<NameEntryScene>());
		SceneManager::Instance().AddScene(std::make_unique<SoloHighscoreScene>());
		SceneManager::Instance().AddScene(std::make_unique<TestingScene>());
		SceneManager::Instance().AddScene(std::make_unique<VersusResultScene>());

		SceneManager::Instance().QueueSceneChange("MainMenu");

		GameStateManager::Instance().SetRunning(true);
		return true;
	}

	void Game::Run()
	{
		Timer::Instance().Start();
		while (GameStateManager::Instance().IsRunning()) {
			Timer::Instance().Update();

			float deltaTime = Timer::Instance().GetDeltaTime();

			ProcessInput();
			Update(deltaTime);
			Render();

			const int fpsLimit = ConfigManager::Instance().GetFpsLimit();
			if (fpsLimit <= 0) continue;

			const int frameDelay = 1000 / fpsLimit;
			const int frameTime = SDL_GetTicks() - Timer::Instance().GetLastTick();

			if (frameDelay > frameTime) 
			{
				SDL_Delay(frameDelay - frameTime);
			}
		}
	}

	void Game::ProcessInput() 
	{
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
		if (GameStateManager::Instance().IsPaused())
		{
			InputHandler::Instance().Update();
			return;
		}

		ScreenShake::Instance().Update(deltaTime);
		SceneManager::Instance().Update(deltaTime);
	}

	void Game::Render()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SceneManager::Instance().Render();

		DebugDraw::Render(renderer);
		DebugOverlay::Instance().Render(renderer);
		PauseMenu::Instance().Render(renderer);

		SDL_RenderPresent(renderer);
	}

	void Game::Cleanup()
	{
		ConfigManager::Instance().Save();
		PauseMenu::Instance().Close();

		TextureManager::Instance().Clear();
		DebugOverlay::Instance().Shutdown();

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