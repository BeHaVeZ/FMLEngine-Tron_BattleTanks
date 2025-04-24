#include "GameAdmin.h"
#include "GameData.h"
#include "SceneManager.h"

namespace FML
{
    void GameAdmin::UnregisterPlayer(GameObject* player)
    {
        players.erase(std::remove(players.begin(), players.end(), player), players.end());
    }

    void GameAdmin::OnPlayerDestroyed(GameObject* player)
    {
        UnregisterPlayer(player);
        CheckForGameOver();
    }

    void GameAdmin::CheckForGameOver()
    {
        if (players.empty() && GameData::CurrentGameMode == GameData::GameMode::Solo)
        {
            SceneManager::Instance().QueueSceneChange("NameEntry");
        }
    }
}
