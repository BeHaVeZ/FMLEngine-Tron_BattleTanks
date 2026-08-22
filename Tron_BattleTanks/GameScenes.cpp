#include "GameScenes.h"
#include "SceneManager.h"
#include "MainMenuScene.h"
#include "SoloScene.h"
#include "SoloLevel2.h"
#include "SoloLevel3.h"
#include "Level404.h"
#include "VersusScene.h"
#include "CoopScene.h"
#include "NameEntryScene.h"
#include "SoloHighscoreScene.h"
#include "TestingScene.h"
#include "VersusResultScene.h"
#include "CoopResultScene.h"
#include "AIShowcaseScene.h"
#include "SandboxScene.h"

namespace FML::GameScenes
{
	void Register()
	{
		auto& scenes = SceneManager::Instance();
		scenes.AddScene(std::make_unique<MainMenuScene>());
		scenes.AddScene(std::make_unique<SoloScene>());
		scenes.AddScene(std::make_unique<SoloLevel2>());
		scenes.AddScene(std::make_unique<SoloLevel3>());
		scenes.AddScene(std::make_unique<Level404>());
		scenes.AddScene(std::make_unique<VersusScene>());
		scenes.AddScene(std::make_unique<CoopScene>());
		scenes.AddScene(std::make_unique<NameEntryScene>());
		scenes.AddScene(std::make_unique<SoloHighscoreScene>());
		scenes.AddScene(std::make_unique<TestingScene>());
		scenes.AddScene(std::make_unique<VersusResultScene>());
		scenes.AddScene(std::make_unique<CoopResultScene>());
		scenes.AddScene(std::make_unique<AIShowcaseScene>());
		scenes.AddScene(std::make_unique<SandboxScene>());

		scenes.QueueSceneChange("MainMenu");
	}
}
