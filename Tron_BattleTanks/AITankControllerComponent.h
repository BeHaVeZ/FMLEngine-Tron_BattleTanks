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
		enum class ShotOutcome
		{
			Miss,
			HitTarget,
			HitSelf,
			HitAlly,
		};

		struct ShotResult
		{
			ShotOutcome outcome{ ShotOutcome::Miss };
			int bounces{ 0 };
			float pathLength{ 0.f };
		};

		struct FiringSolution
		{
			float aimAngle{ 0.f };
			int bounces{ 0 };
			bool valid{ false };
		};

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

		void BuildWallCache();
		ShotResult SimulateShot(const glm::vec2& origin, const glm::vec2& direction, int maxBounces,
			std::vector<glm::vec2>* outPath = nullptr) const;
		bool FindWallBounce(const glm::vec2& point, const glm::vec2& direction, glm::vec2& outNormal, float& outDepth) const;
		glm::vec2 MuzzlePoint(const glm::vec2& forward) const;

		void UpdateFiringSolution(const glm::vec2& position, float deltaTime);
		float DirectAimAngle(const glm::vec2& position, float flightTime) const;
		float AimTargetAngle() const;

		void UpdateAim(const glm::vec2& position, float deltaTime);
		void TryFire();
		bool BarrelShot(glm::vec2& outMuzzle, glm::vec2& outForward) const;
		void RenderPrediction();

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

		std::vector<SDL_Rect> wallRects;
		std::vector<std::vector<int>> wallBuckets;
		bool wallCacheBuilt{ false };

		FiringSolution solution;
		ShotOutcome lastDirectOutcome{ ShotOutcome::Miss };
		float solutionTimer{ 0.f };
		std::vector<glm::vec2> debugPath;
		int selfBlockedCount{ 0 };

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
		static constexpr float simStep = 250.f / 60.f;
		static constexpr float simMaxPath = 1400.f;
		static constexpr float bulletHalfWidth = 3.f;
		static constexpr float bulletHalfHeight = 2.5f;
		static constexpr int bulletBoxWidth = 6;
		static constexpr int bulletBoxHeight = 5;
		static constexpr float separationBias = 1.f;
		static constexpr float muzzleOffset = 28.f;
		static constexpr int bucketSize = 64;
		static constexpr float selfMarginMax = 56.f;
		static constexpr float selfMarginScale = .55f;
		static constexpr int bucketColumns = 1024 / bucketSize + 1;
		static constexpr int bucketRows = 768 / bucketSize + 1;
		static constexpr glm::vec2 labelOffset{ -22.f, -40.f };
	};
}
