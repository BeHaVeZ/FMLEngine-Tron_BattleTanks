#pragma once
#include "TankLevelScene.h"
#include <array>
#include <glm.hpp>

namespace FML
{
	class SandboxScene final : public TankLevelScene
	{
	public:
		SandboxScene();

		bool Initialize(SDL_Renderer* renderer) override;
		void InitializeInput() override;
		void HandleInput(SDL_Event& event) override;

	protected:
		void InitializeUI() override;
		void InitializeManagers() override;

	private:
		enum class SpawnType
		{
			BlueTank,
			PinkTank,
			Recognizer,
			AIAlly,
			TargetDummy,
			EnemyBullet,
			Explosion,
		};

		struct SandboxMap
		{
			const char* name;
			const char* backgroundPath;
			const char* collisionPath;
		};

		static constexpr std::array<SandboxMap, 3> maps{ {
			{ "LEVEL00", "data/levels/level00.png", "data/levels/level00C.txt" },
			{ "LEVEL01", "data/levels/level01.png", "data/levels/level01C.txt" },
			{ "LEVEL02", "data/levels/level02.png", "data/levels/level02C.txt" },
		} };

		static constexpr float tankRadius = 16.f;

		void CycleSpawnType(int direction);
		void CycleAllyDifficulty();
		void CycleMap();
		void ToggleAutoSpawn();
		void SpawnWave();
		void RefreshSpawnLabel();
		void RefreshInfoLabel();
		void SpawnAt(const glm::vec2& position);
		void TeleportPlayerTo(const glm::vec2& position);
		void ClearNpcs();
		void ClearBullets();

		SpawnType selectedSpawnType{ SpawnType::BlueTank };
		size_t mapIndex{ 0 };
	};
}
