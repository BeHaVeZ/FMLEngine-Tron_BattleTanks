#pragma once
#include "Component.h"
#include "AIDifficultyProfile.h"
#include <glm.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace FML
{
	class GameObject;
	class TransformComponent;
	class TurretAimComponent;
	class ShootComponent;

	class AITankControllerComponent final : public Component
	{
	public:
		enum class Stance
		{
			Press,
			Hold,
			Flank,
		};

		explicit AITankControllerComponent(GameData::AIDifficulty difficulty);

		void Initialize() override;
		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

	private:
		struct Threat
		{
			glm::vec2 impactPoint{ 0.f, 0.f };
			glm::vec2 travelDirection{ 0.f, 0.f };
			float timeToImpact{ 0.f };
		};

		GameObject* AcquireTarget() const;

		bool ScanForThreats(const glm::vec2& position, Threat& outThreat);
		bool PredictBullet(const glm::vec2& origin, const glm::vec2& direction, float speed, int bouncesLeft,
			const glm::vec2& position, Threat& outThreat) const;

		void UpdateStance(const glm::vec2& position, float deltaTime);
		glm::vec2 ChooseGoal(const glm::vec2& position) const;
		glm::vec2 DodgeDestination(const glm::vec2& position, const Threat& threat) const;

		void StepAlongPath(const glm::vec2& position, float deltaTime);
		void Replan(const glm::vec2& position, const glm::vec2& goal);
		void MoveOneAxis(const glm::vec2& position, const glm::vec2& desired, float deltaTime);

		void UpdateAim(const glm::vec2& position, float deltaTime);
		void TryFire(const glm::vec2& position);

		void SampleTargetBehaviour(const glm::vec2& position, float deltaTime);

		const AIDifficultyProfile& profile;

		TransformComponent* transform{ nullptr };
		GameObject* turret{ nullptr };
		TurretAimComponent* turretAim{ nullptr };
		ShootComponent* shooting{ nullptr };

		std::vector<glm::vec2> path;
		std::vector<glm::vec2> obstacles;
		size_t nextWaypoint{ 0 };
		glm::vec2 goal{ 0.f, 0.f };
		glm::vec2 lastPosition{ 0.f, 0.f };
		bool hasGoal{ false };

		glm::vec2 targetPosition{ 0.f, 0.f };
		glm::vec2 targetVelocity{ 0.f, 0.f };
		glm::vec2 lastTargetPosition{ 0.f, 0.f };
		bool hasTarget{ false };

		glm::vec2 dodgeDestination{ 0.f, 0.f };
		float dodgeTimer{ 0.f };
		bool dodging{ false };

		float reactionTimer{ 0.f };
		float replanTimer{ 0.f };
		float decisionTimer{ 0.f };
		float aimBias{ 0.f };
		float aimBiasTimer{ 0.f };

		float targetApproachRate{ 0.f };
		float targetFireRate{ 0.f };
		float lastRangeToTarget{ 0.f };
		float damageMemory{ 0.f };
		int lastSeenHealth{ 0 };
		size_t lastSeenBulletCount{ 0 };

		Stance stance{ Stance::Press };
		int threatCount{ 0 };

		static constexpr float agentRadius = 16.f;
		static constexpr float moveSpeed = 100.f;
		static constexpr float hitRadius = 18.f;
		static constexpr float replanInterval = .4f;
		static constexpr float goalMovedThreshold = 40.f;
		static constexpr float waypointRadius = 6.f;
		static constexpr float teleportThreshold = 120.f;
		static constexpr float dodgeDuration = .6f;
		static constexpr float bulletSpeed = 250.f;
		static constexpr int bulletMaxBounces = 4;
		static constexpr float predictionStep = 4.f;
		static constexpr float sightRange = 1000.f;
		static constexpr float aimBiasInterval = 1.2f;
		static constexpr float flankOffset = 140.f;
		static constexpr float minEngageRange = 78.f;
		static constexpr std::string_view teleportTag = "TPCenter";
		static constexpr glm::vec2 labelOffset{ -22.f, -40.f };
	};
}
