#include "GameAdmin.h"
#include "GameData.h"
#include "SceneManager.h"
#include "GameTags.h"

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

	int GameAdmin::WinnerNumber() const
	{
		if (players.empty() || !players.front())
		{
			return 0;
		}

		const std::string_view survivor = players.front()->GetTag();
		if (survivor == Tags::Player1) return 1;
		if (survivor == Tags::Player2) return 2;
		return 0;
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
			if (players.size() <= 1)
			{
				GameData::VersusWinner = WinnerNumber();
				SceneManager::Instance().QueueSceneChangeWithDelay("VersusResult", 2.5f);
			}
			break;
		default:
			break;
		}
	}
}
