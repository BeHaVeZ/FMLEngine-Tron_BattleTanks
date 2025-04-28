#include "TeleportManager.h"
#include <random>

namespace FML
{
    void TeleportManager::InitializeCustomTeleportPositions(const std::vector<glm::vec2>& points)
    {
        teleportPositions = points;
    }

    glm::vec2 TeleportManager::GetRandomTeleportPosition() const
    {
        if (teleportPositions.empty()) return { 0, 0 };

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, teleportPositions.size() - 1);

        return teleportPositions[dist(gen)];
    }
}
