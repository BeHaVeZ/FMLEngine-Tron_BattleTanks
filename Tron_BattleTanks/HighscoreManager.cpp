#include "HighscoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace FML
{
    HighscoreManager::HighscoreManager(const std::string& filePath) : filePath(filePath),currentScore() {}

    std::vector<HighscoreEntry> HighscoreManager::GetTopScores(size_t maxCount)
    {
        std::vector<HighscoreEntry> entries;
        std::ifstream file(filePath);

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string name;
            int score;

            if (iss >> name >> score)
            {
                entries.push_back({ name, score });
            }
        }

        std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
            return b.score < a.score;
            });

        if (entries.size() > maxCount)
            entries.resize(maxCount);

        return entries;
    }
}
