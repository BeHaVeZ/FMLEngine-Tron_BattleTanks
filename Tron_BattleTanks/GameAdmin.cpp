#include "GameAdmin.h"
#include "GameData.h"
#include "SceneManager.h"

namespace FML
{
	void GameAdmin::RegisterPlayer(GameObject* player)
	{
		players.push_back(player);
	}
	void GameAdmin::UnregisterPlayer(GameObject* player)
	{
		players.erase(std::remove(players.begin(), players.end(), player), players.end());
	}

	void GameAdmin::ResetPlayers()
	{
		players.clear();
	}

	void GameAdmin::OnPlayerDestroyed(GameObject* player)
	{
		UnregisterPlayer(player);
		CheckForGameOver();
	}

	void GameAdmin::CheckForGameOver()
	{
		switch (GameData::CurrentGameMode)
		{
		case GameData::GameMode::Solo:
			if (players.empty())
			{
				SceneManager::Instance().QueueSceneChangeWithDelay("NameEntry",2.5f);
			}
			break;
		case GameData::GameMode::Coop:
			if (players.empty())
			{
				SceneManager::Instance().QueueSceneChangeWithDelay("MainMenu",2.5f);
				//Game over screen and show score WIP
			}
			break;
		case GameData::GameMode::Versus:
			//WIP
			SceneManager::Instance().QueueSceneChangeWithDelay("MainMenu",2.5f);
			break;
		default:
			break;
		}
	}
}
