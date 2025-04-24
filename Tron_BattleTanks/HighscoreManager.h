#pragma once
#include <vector>
#include <string>

namespace FML
{
    struct HighscoreEntry
    {
        std::string name;
        int score;
    };

    class HighscoreManager
    {
    public:
        explicit HighscoreManager(const std::string& filePath);

        std::vector<HighscoreEntry> GetTopScores(size_t maxCount = 10);
        int GetHighestScore() const;

        void SetCurrentScore(int score) { currentScore = score; }

    private:
        std::string filePath;
        int currentScore;
    };
}
