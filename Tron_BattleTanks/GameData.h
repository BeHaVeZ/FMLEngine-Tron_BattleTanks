#pragma once

namespace GameData 
{
    enum class GameMode
    {
        None = -1,
        Solo,
        Coop,
        Versus,
    };

    inline GameMode CurrentGameMode = GameMode::None;
    inline int CurrentScore = 0;
};
