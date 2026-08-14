#pragma once
#include <glm.hpp>
#include <random>

namespace FML
{
	class GameObject;

	class GridMovement
	{
	public:
		explicit GridMovement(float moveSpeed);

		void Wander(GameObject* agent, float deltaTime);

		void Seek(GameObject* agent, const glm::vec2& target, float deltaTime);

		void SetSpeedMultiplier(float multiplier) { speedMultiplier = multiplier; }

		glm::vec2 GetForward() const { return up; }
		glm::vec2 GetCenter() const { return center; }

		void DebugRenderWhiskers() const;

	private:
		void CacheFrameData(GameObject* agent);
		void MoveForward(GameObject* agent, float deltaTime) const;

		bool FrontClear(GameObject* agent) const;
		bool LeftSideClear() const;
		bool RightSideClear() const;
		bool SideClear(const glm::vec2& topStart, const glm::vec2& bottomStart, const glm::vec2& middleStart, const glm::vec2& direction) const;

		void TurnRandomly(GameObject* agent, bool leftClear, bool rightClear);
		bool TurnTowards(GameObject* agent, const glm::vec2& target, bool leftClear, bool rightClear, bool allowReverse);
		static void Turn(GameObject* agent, float degrees);

		void ResetTurnCooldown();

		float moveSpeed;
		float speedMultiplier{ 1.f };
		float turnCooldown{ 0.f };

		glm::vec2 up{};
		glm::vec2 right{};
		glm::vec2 center{};
		glm::vec2 topLeft{};
		glm::vec2 topRight{};
		glm::vec2 bottomLeft{};
		glm::vec2 bottomRight{};
		glm::vec2 middleLeft{};
		glm::vec2 middleRight{};
		glm::vec2 rearOffset{};

		std::mt19937 rng;
		std::uniform_int_distribution<int> coinFlip{ 0, 1 };

		bool lastFrontClear{ true };
		bool lastLeftClear{ true };
		bool lastRightClear{ true };

		static constexpr float sideCheckDistance = 50.f;
		static constexpr float frontCheckDistance = 10.f;
		static constexpr float cornerInset = 5.f;
		static constexpr float minTurnCooldown = .1f;
		static constexpr float maxTurnCooldown = 5.f;
		static constexpr float quarterTurn = 90.f;
		static constexpr float halfTurn = 180.f;
	};
}
