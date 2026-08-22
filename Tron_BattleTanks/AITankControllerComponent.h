#pragma once
#include "Component.h"
#include "AIDifficultyProfile.h"
#include <glm.hpp>
#include <random>
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

		struct TankBox
		{
			SDL_Rect box{ 0, 0, 0, 0 };
			ShotOutcome outcome{ ShotOutcome::Miss };
			glm::vec2 offset{ 0.f, 0.f };
			glm::vec2 velocity{ 0.f, 0.f };
		};

		void GatherSceneObjects();
		GameObject* AcquireTarget() const;

		bool ScanForThreats(const glm::vec2& position, Threat& outThreat);
		bool PredictBullet(const glm::vec2& origin, const glm::vec2& direction, float speed, int bouncesLeft,
			const glm::vec2& position, Threat& outThreat) const;

		void UpdateStance(const glm::vec2& position, float deltaTime);
		glm::vec2 ChooseGoal(const glm::vec2& position) const;
		bool EscapeDirection(const glm::vec2& position, const Threat& threat, int blockedMask, int preferredIndex, int& outIndex) const;
		bool PathIsClear(const glm::vec2& from, const glm::vec2& axis, float distance) const;
		static glm::vec2 EscapeAxis(int index);
		bool TryStrafe(const glm::vec2& position, float deltaTime);
		bool TryKite(const glm::vec2& position, float deltaTime);
		void GatherShotContext();

		void StepAlongPath(const glm::vec2& position, float deltaTime);
		void Replan(const glm::vec2& position, const glm::vec2& goal);
		void MoveOneAxis(const glm::vec2& position, const glm::vec2& desired, float deltaTime);

		void BuildWallCache();
		ShotResult SimulateShot(const glm::vec2& origin, const glm::vec2& direction, int maxBounces,
			std::vector<glm::vec2>* outPath = nullptr) const;
		bool FindWallBounce(const glm::vec2& point, const glm::vec2& direction, glm::vec2& outNormal, float& outDepth) const;
		bool StepBullet(glm::vec2& point, glm::vec2& heading, int& bouncesLeft) const;
		glm::vec2 MuzzlePoint(const glm::vec2& forward) const;

		void UpdateFiringSolution(const glm::vec2& position, float deltaTime);
		glm::vec2 LeadVelocity() const;
		glm::vec2 PredictedTargetPosition(float time) const;
		float InterceptTime(const glm::vec2& position) const;
		float DirectAimAngle(const glm::vec2& position) const;
		float AimTargetAngle() const;
		void RollAimBias();

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

		GameObject* scenePlayer1{ nullptr };
		GameObject* sceneTeleport{ nullptr };
		std::vector<GameObject*> sceneEnemies;
		std::vector<GameObject*> sceneBullets;

		std::vector<glm::vec2> path;
		std::vector<glm::vec2> obstacles;
		size_t nextWaypoint{ 0 };
		glm::vec2 goal{ 0.f, 0.f };
		glm::vec2 lastPosition{ 0.f, 0.f };
		bool hasGoal{ false };

		glm::vec2 targetPosition{ 0.f, 0.f };
		glm::vec2 targetVelocity{ 0.f, 0.f };
		glm::vec2 lastTargetPosition{ 0.f, 0.f };
		GameObject* targetObject{ nullptr };
		bool hasTarget{ false };
		bool targetInvulnerable{ false };

		std::vector<TankBox> shotTanks;
		SDL_Rect shotSelfBox{ 0, 0, 0, 0 };
		SDL_Rect shotTeleportBox{ 0, 0, 0, 0 };
		bool shotSelfValid{ false };
		bool shotTeleportValid{ false };
		int pendingOwnShots{ 0 };
		bool kiting{ false };

		glm::vec2 dodgeAxis{ 0.f, 0.f };
		glm::vec2 dodgeTarget{ 0.f, 0.f };
		glm::vec2 dodgeProgressPosition{ 0.f, 0.f };
		glm::vec2 progressPosition{ 0.f, 0.f };
		float progressTimer{ 0.f };
		float intendedTravel{ 0.f };
		glm::vec2 heldMoveAxis{ 0.f, 0.f };
		float axisHoldTimer{ 0.f };
		bool heldMoveAxisValid{ false };
		float dodgeHold{ 0.f };
		float dodgeStuckTimer{ 0.f };
		int dodgeAxisIndex{ -1 };
		int preferredDodgeAxis{ -1 };
		int dodgeBlockedMask{ 0 };
		bool dodging{ false };

		float reactionTimer{ 0.f };
		float replanTimer{ 0.f };
		float decisionTimer{ 0.f };
		float aimBias{ 0.f };
		float leadBias{ 1.f };
		float fireDelayTimer{ 0.f };
		float strafeTimer{ 0.f };
		float strafeSign{ 1.f };
		std::mt19937 rng{ std::random_device{}() };

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
		bool bankScanActive{ false };
		float bankScanAngle{ 0.f };
		FiringSolution bankBest;
		float bankBestPath{ 0.f };
		std::vector<glm::vec2> debugPath;
		int selfBlockedCount{ 0 };
		int dodgeCount{ 0 };
		int dodgeStallCount{ 0 };
		int escapeFailCount{ 0 };
		int movementStallCount{ 0 };
		int dodgeReversalCount{ 0 };
		int axisFlipCount{ 0 };
		int kiteCount{ 0 };
		int threatSeenCount{ 0 };

		static constexpr float agentRadius = 16.f;
		static constexpr float moveSpeed = 100.f;
		static constexpr float hitRadius = 22.f;
		static constexpr float replanInterval = .4f;
		static constexpr float goalMovedThreshold = 40.f;
		static constexpr float waypointRadius = 6.f;
		static constexpr float teleportThreshold = 120.f;
		static constexpr float dodgeHoldTime = .15f;
		static constexpr float dodgeReactWindow = .5f;
		static constexpr float escapeClearance = 26.f;
		static constexpr float escapeProbe = 40.f;
		static constexpr float bodyProbe = agentRadius - 3.f;
		static constexpr int escapeAxisCount = 4;
		static constexpr float dodgeStuckWindow = .12f;
		static constexpr float dodgeStuckDistance = 6.f;
		static constexpr float dodgeStickiness = 30.f;
		static constexpr float moveProbe = 14.f;
		static constexpr float axisReleaseDistance = 6.f;
		static constexpr float minAxisHold = .22f;
		static constexpr float progressWindow = .4f;
		static constexpr float progressDistance = 8.f;
		static constexpr float strafeFlipInterval = 1.1f;
		static constexpr float strafeProbe = 44.f;
		static constexpr float bulletSpeed = 250.f;
		static constexpr int bulletMaxBounces = 4;
		static constexpr int bankAnglesPerFrame = 24;
		static constexpr int interceptIterations = 3;

		static constexpr float kiteRadius = 150.f;
		static constexpr float flankOffset = 140.f;
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
