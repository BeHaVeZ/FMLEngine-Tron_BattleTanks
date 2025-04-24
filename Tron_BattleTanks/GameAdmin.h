#pragma once
#include <vector>
#include "GameObject.h"

namespace FML
{
    class GameAdmin
    {
    public:
        static GameAdmin& Instance()
        {
            static GameAdmin instance;
            return instance;
        }

        void RegisterPlayer(GameObject* player) { players.push_back(player); }
        void UnregisterPlayer(GameObject* player);

        void OnPlayerDestroyed(GameObject* player);
        void CheckForGameOver();

    private:
        std::vector<GameObject*> players;
    };
}
