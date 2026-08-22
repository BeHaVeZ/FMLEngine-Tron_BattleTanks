#pragma once
#define SDL_MAIN_HANDLED
#include <SDL.h>

namespace FML
{
	class Game 
	{
	public:
		Game();
		~Game();

		bool Initialize(const char* windowTitle);

		void Run();

		void ProcessInput();

		void Update(float deltaTime);

		void Render();

		void Cleanup();

		bool IsRunning() const { return isRunning; }

	private:
		SDL_Window* window;
		SDL_Renderer* renderer;

		bool isRunning;
	};
}

