#include "RecognizerMovementHelper.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "CollisionManager.h"
#include "Logger.h"
#include <cmath>
#include <random>

namespace FML
{
    glm::vec2 RecognizerMovementHelper::up;
    glm::vec2 RecognizerMovementHelper::right;
    glm::vec2 RecognizerMovementHelper::center;
    glm::vec2 RecognizerMovementHelper::bottomLeft;
    glm::vec2 RecognizerMovementHelper::middleLeft;
    glm::vec2 RecognizerMovementHelper::topLeft;
    glm::vec2 RecognizerMovementHelper::bottomRight;
    glm::vec2 RecognizerMovementHelper::middleRight;
    glm::vec2 RecognizerMovementHelper::topRight;
    glm::vec2 RecognizerMovementHelper::off;

    float RecognizerMovementHelper::turnCooldown = 0.f;

    void RecognizerMovementHelper::Move(GameObject* recognizer, float moveSpeed)
    {
        CacheFrameData(recognizer);

        bool frontClear = FrontClear(recognizer);
        bool leftClear = LeftSideClear(recognizer);
        bool rightClear = RightSideClear(recognizer);

        turnCooldown -= Timer::Instance().GetDeltaTime();

        if (!frontClear)
        {
            DecideTurn(recognizer, leftClear, rightClear);
            ResetCooldownTimer();
        }
        else if (turnCooldown <= 0.f && (leftClear || rightClear))
        {
            DecideTurn(recognizer, leftClear, rightClear);
            ResetCooldownTimer();
        }

        auto* transform = recognizer->GetComponent<TransformComponent>();
        if (!transform) return;

        glm::vec2 currentPos = transform->GetLocalPosition();
        glm::vec2 newPos = currentPos + up * moveSpeed * Timer::Instance().GetDeltaTime();
        transform->SetPosition(newPos);
    }

    void RecognizerMovementHelper::ResetCooldownTimer()
    {
        static std::random_device rd;
        static std::mt19937 rng(rd());
        std::uniform_real_distribution<float> dist(minTurnCooldownTime, maxTurnCooldownTime);
        turnCooldown = dist(rng);
    }

    void RecognizerMovementHelper::CacheFrameData(GameObject* recognizer)
    {
        auto* transform = recognizer->GetComponent<TransformComponent>();
        auto* texture = recognizer->GetComponent<TextureComponent>();
        if (!transform || !texture)
            return;

        float worldRotationDegrees = transform->GetWorldRotation();
        float rotationRadians = glm::radians(worldRotationDegrees + 90.f);

        up = { -std::cos(rotationRadians), -std::sin(rotationRadians) };
        right = { -up.y, up.x };
        center = transform->GetWorldPosition();

        float halfWidth = texture->GetDefaultWidth() * 0.5f;
        float halfHeight = texture->GetDefaultHeight() * 0.5f;

        bottomLeft = center - right * halfWidth - up * halfHeight;
        topLeft = center - right * halfWidth + up * halfHeight;
        bottomRight = center + right * halfWidth - up * halfHeight;
        topRight = center + right * halfWidth + up * halfHeight;

        middleLeft = center - right + up;
        middleRight = center + right + up;

        off = -up * offsetDistance;
    }

    bool RecognizerMovementHelper::FrontClear(GameObject* recognizer)
    {
        const bool wallInFront =
            CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, "Wall") ||
            CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, "Wall");

        bool enemyInFront = false;
        std::vector<std::string> enemyTags = { "BlueTank", "PinkTank", "Recognizer" };
        for (const auto& tag : enemyTags)
        {
            enemyInFront |=
                CollisionManager::Instance().RaycastWithTag(topLeft, up, 10.f, tag, recognizer) ||
                CollisionManager::Instance().RaycastWithTag(topRight, up, 10.f, tag, recognizer);
        }

        return !(wallInFront || enemyInFront);
    }

    bool RecognizerMovementHelper::LeftSideClear(GameObject* recognizer)
    {
        glm::vec2 topRayStart = topLeft - off;
        glm::vec2 bottomRayStart = bottomLeft + off;
        glm::vec2 middleRayStart = middleLeft;

        bool topClear = !CollisionManager::Instance().RaycastWithTag(topRayStart, -right, checkDistance, "Wall");
        bool bottomClear = !CollisionManager::Instance().RaycastWithTag(bottomRayStart, -right, checkDistance, "Wall");
        bool middleClear = !CollisionManager::Instance().RaycastWithTag(middleRayStart, -right, checkDistance, "Wall");

        return topClear && bottomClear && middleClear;
    }

    bool RecognizerMovementHelper::RightSideClear(GameObject* recognizer)
    {
        glm::vec2 topRayStart = topRight - off;
        glm::vec2 bottomRayStart = bottomRight + off;
        glm::vec2 middleRayStart = middleRight;

        bool topClear = !CollisionManager::Instance().RaycastWithTag(topRayStart, right, checkDistance, "Wall");
        bool bottomClear = !CollisionManager::Instance().RaycastWithTag(bottomRayStart, right, checkDistance, "Wall");
        bool middleClear = !CollisionManager::Instance().RaycastWithTag(middleRayStart, right, checkDistance, "Wall");

        return topClear && bottomClear && middleClear;
    }

    void RecognizerMovementHelper::DecideTurn(GameObject* recognizer, bool leftClear, bool rightClear)
    {
        static std::random_device rd;
        static std::mt19937 rng(rd());
        static std::uniform_int_distribution<int> flipCoinDistribution(0, 1);

        int randomSide = flipCoinDistribution(rng); // 0 = LEFT  1 = RIGHT

        if (leftClear && rightClear)
        {
            if (randomSide == 0)
                TurnLeft(recognizer);
            else
                TurnRight(recognizer);
        }
        else if (leftClear)
        {
            TurnLeft(recognizer);
        }
        else if (rightClear)
        {
            TurnRight(recognizer);
        }
        else
        {
            TurnBack(recognizer);
        }
    }

    void RecognizerMovementHelper::TurnLeft(GameObject* recognizer)
    {
        auto* transform = recognizer->GetComponent<TransformComponent>();
        if (!transform) return;
        transform->SetRotation(transform->GetLocalRotation() + 90);
    }

    void RecognizerMovementHelper::TurnRight(GameObject* recognizer)
    {
        auto* transform = recognizer->GetComponent<TransformComponent>();
        if (!transform) return;
        transform->SetRotation(transform->GetLocalRotation() - 90);
    }

    void RecognizerMovementHelper::TurnBack(GameObject* recognizer)
    {
        auto* transform = recognizer->GetComponent<TransformComponent>();
        if (!transform) return;
        transform->SetRotation(transform->GetLocalRotation() - 180);
    }

    bool RecognizerMovementHelper::PlayerVisible(GameObject* recognizer)
    {
        auto transform = recognizer->GetComponent<TransformComponent>();
        auto texture = recognizer->GetComponent<TextureComponent>();
        float halfWidth = texture->GetDefaultWidth() / 2.f;
        float halfHeight = texture->GetDefaultHeight() / 2.f;
        float worldRotationDegrees = transform->GetWorldRotation();
        float rotationRadians = glm::radians(worldRotationDegrees + 90.f);
        glm::vec2 up = { -std::cos(rotationRadians), -std::sin(rotationRadians) };
        glm::vec2 right = { up.y, -up.x };
        glm::vec2 center = transform->GetWorldPosition();
        glm::vec2 topLeft = center - right * halfWidth + up * halfHeight;
        glm::vec2 topRight = center + right * halfWidth + up * halfHeight;
        glm::vec2 shootDir = up;

        auto leftHit = CollisionManager::Instance()
            .RaycastFirstHit(topLeft, shootDir, 1000.f, recognizer, nullptr);
        auto rightHit = CollisionManager::Instance()
            .RaycastFirstHit(topRight, shootDir, 1000.f, recognizer, nullptr);

        if (IsPlayerHit(leftHit) || IsPlayerHit(rightHit))
            return true;
        else
            return false;

    };

    bool RecognizerMovementHelper::IsPlayerHit(const std::optional<CollisionManager::RaycastHit>& hit)
    {
        return hit && (hit->hitObject->GetTag() == "Player1" || hit->hitObject->GetTag() == "Player2");
    }

}
