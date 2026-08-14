#include "GridMovement.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "GameTags.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include "CollisionManager.h"
#include <array>
#include <cmath>

namespace FML
{
	GridMovement::GridMovement(float speed)
		: moveSpeed(speed)
	{
		std::random_device rd;
		rng = std::mt19937(rd());
	}

	void GridMovement::Wander(GameObject* agent, float deltaTime)
	{
		turnCooldown -= deltaTime;
		CacheFrameData(agent);

		const bool leftClear = LeftSideClear();
		const bool rightClear = RightSideClear();
		lastLeftClear = leftClear;
		lastRightClear = rightClear;
		lastFrontClear = FrontClear(agent);

		if (!lastFrontClear)
		{
			TurnRandomly(agent, leftClear, rightClear);
			ResetTurnCooldown();
			CacheFrameData(agent);
		}
		else if (turnCooldown <= 0.f && (leftClear || rightClear))
		{
			TurnRandomly(agent, leftClear, rightClear);
			ResetTurnCooldown();
			CacheFrameData(agent);
		}

		MoveForward(agent, deltaTime);
	}

	void GridMovement::Seek(GameObject* agent, const glm::vec2& target, float deltaTime)
	{
		CacheFrameData(agent);

		const bool leftClear = LeftSideClear();
		const bool rightClear = RightSideClear();
		const bool frontClear = FrontClear(agent);
		lastLeftClear = leftClear;
		lastRightClear = rightClear;
		lastFrontClear = frontClear;

		bool turned = false;
		if (!frontClear)
		{
			turned = TurnTowards(agent, target, leftClear, rightClear, true);
		}
		else
		{
			const glm::vec2 toTarget = target - center;
			if (std::abs(glm::dot(toTarget, right)) > glm::dot(toTarget, up))
			{
				turned = TurnTowards(agent, target, leftClear, rightClear, false);
			}
		}

		if (turned)
		{
			CacheFrameData(agent);
		}

		MoveForward(agent, deltaTime);
	}

	void GridMovement::CacheFrameData(GameObject* agent)
	{
		const auto* transform = agent->GetComponent<TransformComponent>();
		const auto* texture = agent->GetComponent<TextureComponent>();
		if (!transform || !texture)
			return;

		const float rotationRadians = glm::radians(transform->GetWorldRotation() + 90.f);

		up = { -std::cos(rotationRadians), -std::sin(rotationRadians) };
		right = { -up.y, up.x };
		center = transform->GetWorldPosition();

		const float halfWidth = texture->GetDefaultWidth() * 0.5f;
		const float halfHeight = texture->GetDefaultHeight() * 0.5f;

		bottomLeft = center - right * halfWidth - up * halfHeight;
		topLeft = center - right * halfWidth + up * halfHeight;
		bottomRight = center + right * halfWidth - up * halfHeight;
		topRight = center + right * halfWidth + up * halfHeight;

		middleLeft = center - right + up;
		middleRight = center + right + up;

		rearOffset = -up * cornerInset;
	}

	void GridMovement::MoveForward(GameObject* agent, float deltaTime) const
	{
		auto* transform = agent->GetComponent<TransformComponent>();
		if (!transform)
			return;

		transform->SetPosition(transform->GetLocalPosition() + up * moveSpeed * speedMultiplier * deltaTime);
	}

	bool GridMovement::FrontClear(GameObject* agent) const
	{
		auto& collisions = CollisionManager::Instance();
		const auto blocked = [&](const std::string_view tag, GameObject* exclude)
			{
				return collisions.RaycastWithTag(topLeft, up, frontCheckDistance, tag, exclude)
					|| collisions.RaycastWithTag(topRight, up, frontCheckDistance, tag, exclude);
			};

		if (blocked(Tags::Wall, nullptr) || blocked(Tags::Player1, nullptr) || blocked(Tags::Player2, nullptr))
			return false;

		static constexpr std::array enemyTags{ Tags::BlueTank, Tags::PinkTank, Tags::Recognizer };
		for (const auto& tag : enemyTags)
		{
			if (blocked(tag, agent))
				return false;
		}

		return true;
	}

	bool GridMovement::SideClear(const glm::vec2& topStart, const glm::vec2& bottomStart, const glm::vec2& middleStart, const glm::vec2& direction) const
	{
		auto& collisions = CollisionManager::Instance();
		return !collisions.RaycastWithTag(topStart, direction, sideCheckDistance, Tags::Wall)
			&& !collisions.RaycastWithTag(bottomStart, direction, sideCheckDistance, Tags::Wall)
			&& !collisions.RaycastWithTag(middleStart, direction, sideCheckDistance, Tags::Wall);
	}

	bool GridMovement::RightSideClear() const
	{
		return SideClear(topRight - rearOffset, bottomRight + rearOffset, middleRight, right);
	}

	bool GridMovement::LeftSideClear() const
	{
		return SideClear(topLeft - rearOffset, bottomLeft + rearOffset, middleLeft, -right);
	}

	void GridMovement::TurnRandomly(GameObject* agent, bool leftClear, bool rightClear)
	{
		if (leftClear && rightClear)
		{
			Turn(agent, coinFlip(rng) == 0 ? -quarterTurn : quarterTurn);
		}
		else if (leftClear)
		{
			Turn(agent, -quarterTurn);
		}
		else if (rightClear)
		{
			Turn(agent, quarterTurn);
		}
		else
		{
			Turn(agent, halfTurn);
		}
	}

	bool GridMovement::TurnTowards(GameObject* agent, const glm::vec2& target, bool leftClear, bool rightClear, bool allowReverse)
	{
		const bool preferRight = glm::dot(target - center, right) > 0.f;

		const bool preferredClear = preferRight ? rightClear : leftClear;
		const bool fallbackClear = preferRight ? leftClear : rightClear;

		if (preferredClear)
		{
			Turn(agent, preferRight ? quarterTurn : -quarterTurn);
			return true;
		}
		if (fallbackClear)
		{
			Turn(agent, preferRight ? -quarterTurn : quarterTurn);
			return true;
		}
		if (allowReverse)
		{
			Turn(agent, halfTurn);
			return true;
		}
		return false;
	}

	void GridMovement::Turn(GameObject* agent, float degrees)
	{
		auto* transform = agent->GetComponent<TransformComponent>();
		if (!transform)
			return;

		transform->SetRotation(transform->GetLocalRotation() + degrees);
	}

	void GridMovement::DebugRenderWhiskers() const
	{
		if (!DebugEnabled(DebugChannel::Whiskers))
			return;

		constexpr glm::vec4 clearColor{ .2f, 1.f, .3f, .85f };
		constexpr glm::vec4 blockedColor{ 1.f, .25f, .2f, 1.f };

		const glm::vec4 frontColor = lastFrontClear ? clearColor : blockedColor;
		DebugDraw::DrawLine(topLeft, topLeft + up * frontCheckDistance, frontColor);
		DebugDraw::DrawLine(topRight, topRight + up * frontCheckDistance, frontColor);

		const glm::vec4 rightColor = lastRightClear ? clearColor : blockedColor;
		DebugDraw::DrawLine(topRight - rearOffset, topRight - rearOffset + right * sideCheckDistance, rightColor);
		DebugDraw::DrawLine(bottomRight + rearOffset, bottomRight + rearOffset + right * sideCheckDistance, rightColor);
		DebugDraw::DrawLine(middleRight, middleRight + right * sideCheckDistance, rightColor);

		const glm::vec4 leftColor = lastLeftClear ? clearColor : blockedColor;
		DebugDraw::DrawLine(topLeft - rearOffset, topLeft - rearOffset - right * sideCheckDistance, leftColor);
		DebugDraw::DrawLine(bottomLeft + rearOffset, bottomLeft + rearOffset - right * sideCheckDistance, leftColor);
		DebugDraw::DrawLine(middleLeft, middleLeft - right * sideCheckDistance, leftColor);

		DebugDraw::DrawLine(center, center + up * 20.f, { 1.f, 1.f, 1.f, 1.f });
		DebugDraw::DrawLine(center, center + right * 12.f, { 1.f, .6f, .1f, 1.f });
	}

	void GridMovement::ResetTurnCooldown()
	{
		std::uniform_real_distribution<float> distribution(minTurnCooldown, maxTurnCooldown);
		turnCooldown = distribution(rng);
	}
}
