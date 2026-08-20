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

    enum class AIDifficulty
    {
        Easy,
        Normal,
        Hard,
    };

    inline GameMode CurrentGameMode = GameMode::None;
    inline int CurrentScore = 0;
    inline int Player1Health = 3;
    inline int Player2Health = 3;

    inline int VersusWinner = 0;

    inline bool Player2IsAI = false;
    inline AIDifficulty AiDifficulty = AIDifficulty::Normal;

	inline void ResetValues()
	{
		CurrentGameMode = GameMode::None;
		CurrentScore = 0;
        Player1Health = 3;
        Player2Health = 3;
        VersusWinner = 0;
        Player2IsAI = false;
        AiDifficulty = AIDifficulty::Normal;
    }
};
