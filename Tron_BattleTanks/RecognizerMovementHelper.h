#pragma once
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "CollisionManager.h"
#include "Timer.h"
#include "Logger.h"
#include <glm.hpp>
#include <random>

namespace FML
{
    class RecognizerMovementHelper
    {
    public:
        static void Move(GameObject* recognizer, float moveSpeed);

        static bool FrontClear(GameObject* recognizer);
        static bool LeftSideClear(GameObject* recognizer);
        static bool RightSideClear(GameObject* recognizer);

        static void DecideTurn(GameObject* recognizer, bool leftClear, bool rightClear);

        static bool PlayerVisible(GameObject* recognizer);
        static bool IsPlayerHit(const std::optional<CollisionManager::RaycastHit>& hit);

    private:
        static void CacheFrameData(GameObject* recognizer);
        static void TurnLeft(GameObject* recognizer);
        static void TurnRight(GameObject* recognizer);
        static void TurnBack(GameObject* recognizer);

        static glm::vec2 up, right, center;
        static glm::vec2 bottomLeft, middleLeft, topLeft;
        static glm::vec2 bottomRight, middleRight, topRight;
        static glm::vec2 off;

        static constexpr float checkDistance = 50.f;
        static constexpr float offsetDistance = 5.f;

    private:
        static float turnCooldown;
        static constexpr float minTurnCooldownTime = .1f;
        static constexpr float maxTurnCooldownTime = 3.0f;

        static void ResetCooldownTimer();
    };
}
