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

    struct PlayerStats
    {
        int blueTankKills = 0;
        int pinkTankKills = 0;
        int recognizerKills = 0;
        int score = 0;

        int TotalKills() const { return blueTankKills + pinkTankKills + recognizerKills; }
    };

    inline GameMode CurrentGameMode = GameMode::None;
    inline int CurrentScore = 0;
    inline int Player1Health = 3;
    inline int Player2Health = 3;

    inline PlayerStats Player1Stats;
    inline PlayerStats Player2Stats;

    inline int VersusWinner = 0;

    inline bool Player2IsAI = false;
    inline AIDifficulty AiDifficulty = AIDifficulty::Normal;

    inline PlayerStats* StatsForPlayer(int playerNumber)
    {
        if (playerNumber == 1) return &Player1Stats;
        if (playerNumber == 2) return &Player2Stats;
        return nullptr;
    }

	inline void ResetValues()
	{
		CurrentGameMode = GameMode::None;
		CurrentScore = 0;
        Player1Health = 3;
        Player2Health = 3;
        Player1Stats = {};
        Player2Stats = {};
        VersusWinner = 0;
        Player2IsAI = false;
        AiDifficulty = AIDifficulty::Normal;
    }
};
