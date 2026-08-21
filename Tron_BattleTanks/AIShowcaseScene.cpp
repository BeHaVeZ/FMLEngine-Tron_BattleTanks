#include "AIShowcaseScene.h"
#include "AITankControllerComponent.h"
#include "GameData.h"
#include "GameTags.h"
#include "HealthComponent.h"
#include "InputBindingHelper.h"
#include "PrefabRegistry.h"

namespace FML
{
	AIShowcaseScene::AIShowcaseScene()
		: TankLevelScene("AIShowcase", LevelConfig{
			"data/levels/level00.png", "data/levels/level00C.txt", "SoloTheme_1.wav",
			{ 514.f, 428.f }, 3, 2, 1, 1.5f })
	{
	}

	bool AIShowcaseScene::Initialize(SDL_Renderer* renderer)
	{
		GameData::CurrentGameMode = GameData::GameMode::Coop;
		GameData::Player2IsAI = true;
		GameData::AiDifficulty = GameData::AIDifficulty::Hard;
		return TankLevelScene::Initialize(renderer);
	}

	void AIShowcaseScene::InitializePlayer()
	{
		auto tank = PrefabRegistry::Instance().CreateYellowTankPrefab(config.playerSpawn, std::string(Tags::Player2));
		tank->AddComponent(std::make_unique<AITankControllerComponent>(GameData::AIDifficulty::Hard));
		tank->GetComponent<AITankControllerComponent>()->Initialize();

		// The showcase runs unattended; an ally that dies would end the recording.
		if (auto* health = tank->GetComponent<HealthComponent>())
		{
			health->SetInvulnerable(true);
		}

		AddGameObject(std::move(tank));
	}

	void AIShowcaseScene::InitializeUI()
	{
		AddGameObject(PrefabRegistry::Instance().CreateScoreUI(
			{ 580.f, 30.f }, "Score AI", &GameData::Player2Stats.score, SDL_Color{ 255, 255, 0, 255 }, "ScoreUIPlayer2"));
	}

	void AIShowcaseScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();
		InputBindingHelper::BindPauseControls();
	}
}
