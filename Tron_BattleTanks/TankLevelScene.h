#pragma once
#include "Scene.h"
#include <glm.hpp>
#include <string>

namespace FML
{
	struct LevelConfig
	{
		std::string backgroundPath;
		std::string collisionPath;
		std::string musicPath;
		glm::vec2 playerSpawn{ 514.f, 428.f };
		int maxBlueTanks{ 3 };
		int maxPinkTanks{ 2 };
		int maxRecognizers{ 1 };
		float spawnCooldown{ 2.5f };
	};

	class TankLevelScene : public Scene
	{
	public:
		TankLevelScene(std::string sceneName, LevelConfig config);

		bool Initialize(SDL_Renderer* renderer) override;
		void InitializeInput() override;
		void HandleInput(SDL_Event& event) override;
		void Render(SDL_Renderer* renderer) override;
		void OnExit() override;

	private:
		void InitializeBackground(SDL_Renderer* renderer);
		void InitializeFPSCounter(SDL_Renderer* renderer);
		void InitializePlayer();
		void InitializeUI();
		void InitializeWalls();
		void InitializeManagers();
		void InitializeSounds();
		void ReportDebugStats();

		LevelConfig config;

		static constexpr int hudHeight = 100;
		static constexpr int navCellSize = 8;
	};
}
