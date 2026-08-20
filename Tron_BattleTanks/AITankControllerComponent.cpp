#include "AITankControllerComponent.h"
#include "AgentAvoidance.h"
#include "BulletMoveComponent.h"
#include "CollisionManager.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
#include "EnemyPerception.h"
#include "GameData.h"
#include "GameObject.h"
#include "GameTags.h"
#include "HealthComponent.h"
#include "NavGrid.h"
#include "SceneManager.h"
#include "ShootComponent.h"
#include "TransformComponent.h"
#include "TurretAimComponent.h"
#include <algorithm>
#include <cmath>

namespace FML
{
	namespace
	{
		float NormalizeAngle(float degrees)
		{
			degrees = std::fmod(degrees + 180.f, 360.f);
			if (degrees < 0.f)
				degrees += 360.f;

			return degrees - 180.f;
		}

		float AimAngleFor(const glm::vec2& direction)
		{
			return NormalizeAngle(-glm::degrees(std::atan2(direction.y, direction.x)) - 90.f);
		}

		glm::vec2 DirectionForAim(float aim)
		{
			const float radians = glm::radians(-aim - 90.f);
			return { std::cos(radians), std::sin(radians) };
		}

		glm::vec2 DominantAxis(const glm::vec2& direction)
		{
			if (std::abs(direction.x) >= std::abs(direction.y))
				return { direction.x >= 0.f ? 1.f : -1.f, 0.f };

			return { 0.f, direction.y >= 0.f ? 1.f : -1.f };
		}

		float DistanceToSegment(const glm::vec2& point, const glm::vec2& start, const glm::vec2& end)
		{
			const glm::vec2 segment = end - start;
			const float lengthSquared = glm::dot(segment, segment);
			if (lengthSquared <= 0.f)
				return glm::distance(point, start);

			const float t = std::clamp(glm::dot(point - start, segment) / lengthSquared, 0.f, 1.f);
			return glm::distance(point, start + segment * t);
		}

		bool IsBulletTag(std::string_view tag)
		{
			return tag == Tags::Bullet || tag == Tags::EnemyBullet;
		}
	}

	AITankControllerComponent::AITankControllerComponent(GameData::AIDifficulty difficulty)
		: profile(ProfileFor(difficulty))
	{
	}

	void AITankControllerComponent::Initialize()
	{
		if (!gameObject)
			return;

		transform = gameObject->GetComponent<TransformComponent>();
		turret = gameObject->FindChildByTag("Turret");

		if (turret)
		{
			turretAim = turret->GetComponent<TurretAimComponent>();
			shooting = turret->GetComponent<ShootComponent>();
		}

		if (transform)
			lastPosition = transform->GetWorldPosition();

		if (auto* health = gameObject->GetComponent<HealthComponent>())
			lastSeenHealth = health->GetCurrentHealth();

		aimBias = profile.aimError;
		aimBiasTimer = aimBiasInterval;
		decisionTimer = profile.decisionInterval;
	}

	void AITankControllerComponent::Update(float deltaTime)
	{
		if (!transform || !gameObject || gameObject->IsMarkedForDestruction())
			return;

		if (!NavGrid::Instance().IsBuilt())
			return;

		const glm::vec2 position = transform->GetWorldPosition();

		if (glm::distance(position, lastPosition) > teleportThreshold)
		{
			path.clear();
			nextWaypoint = 0;
			hasGoal = false;
			dodging = false;
		}
		lastPosition = position;

		AgentAvoidance::Instance().Register(gameObject, position, agentRadius, AgentAvoidance::Priority::Chasing);

		reactionTimer -= deltaTime;
		if (reactionTimer <= 0.f)
		{
			reactionTimer = profile.reactionDelay;

			if (GameObject* target = AcquireTarget())
			{
				const glm::vec2 seenAt = target->GetComponent<TransformComponent>()->GetWorldPosition();
				if (hasTarget)
					targetVelocity = (seenAt - lastTargetPosition) / std::max(profile.reactionDelay, .016f);

				lastTargetPosition = seenAt;
				targetPosition = seenAt;
				hasTarget = true;
			}
			else
			{
				hasTarget = false;
				targetVelocity = { 0.f, 0.f };
			}
		}

		SampleTargetBehaviour(position, deltaTime);

		aimBiasTimer -= deltaTime;
		if (aimBiasTimer <= 0.f)
		{
			aimBiasTimer = aimBiasInterval;
			aimBias = -aimBias;
		}

		Threat threat{};
		const bool threatened = ScanForThreats(position, threat);

		dodgeTimer -= deltaTime;
		if (threatened && threat.timeToImpact <= profile.threatHorizon)
		{
			const glm::vec2 escape = DodgeDestination(position, threat);
			if (escape != position)
			{
				dodgeDestination = escape;
				dodgeTimer = dodgeDuration * profile.dodgeCommitment;
				dodging = true;
			}
		}

		if (dodgeTimer <= 0.f || glm::distance(position, dodgeDestination) < waypointRadius)
			dodging = false;

		UpdateStance(position, deltaTime);

		const glm::vec2 desiredGoal = dodging ? dodgeDestination : ChooseGoal(position);

		replanTimer -= deltaTime;
		const bool goalMoved = !hasGoal || glm::distance(desiredGoal, goal) > goalMovedThreshold;
		const bool pathDone = nextWaypoint >= path.size();

		if ((goalMoved || pathDone) && (replanTimer <= 0.f || pathDone))
			Replan(position, desiredGoal);

		StepAlongPath(position, deltaTime);
		UpdateAim(position, deltaTime);
		TryFire(position);
	}

	GameObject* AITankControllerComponent::AcquireTarget() const
	{
		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (!scene)
			return nullptr;

		if (GameData::CurrentGameMode == GameData::GameMode::Versus)
		{
			GameObject* human = scene->FindGameObjectByTag(std::string(Tags::Player1));
			return (human && !human->IsMarkedForDestruction()) ? human : nullptr;
		}

		const glm::vec2 position = transform->GetWorldPosition();
		GameObject* closest = nullptr;
		float closestDistance = 0.f;

		for (std::string_view tag : { Tags::BlueTank, Tags::PinkTank, Tags::Recognizer })
		{
			for (GameObject* enemy : scene->FindGameObjectsByTag(tag))
			{
				auto* enemyTransform = enemy->GetComponent<TransformComponent>();
				if (!enemyTransform)
					continue;

				const float distance = glm::distance(position, enemyTransform->GetWorldPosition());
				if (!closest || distance < closestDistance)
				{
					closest = enemy;
					closestDistance = distance;
				}
			}
		}

		return closest;
	}

	bool AITankControllerComponent::ScanForThreats(const glm::vec2& position, Threat& outThreat)
	{
		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (!scene)
			return false;

		bool found = false;
		Threat nearest{};
		int seen = 0;

		for (std::string_view tag : { Tags::Bullet, Tags::EnemyBullet })
		{
			for (GameObject* bullet : scene->FindGameObjectsByTag(tag))
			{
				auto* bulletTransform = bullet->GetComponent<TransformComponent>();
				auto* move = bullet->GetComponent<BulletMoveComponent>();
				if (!bulletTransform || !move)
					continue;

				++seen;

				Threat candidate{};
				if (!PredictBullet(bulletTransform->GetWorldPosition(), move->GetMoveDirection(), move->GetSpeed(),
					move->GetBouncesLeft(), position, candidate))
					continue;

				if (!found || candidate.timeToImpact < nearest.timeToImpact)
				{
					nearest = candidate;
					found = true;
				}
			}
		}

		threatCount = seen;

		if (found)
			outThreat = nearest;

		return found;
	}

	bool AITankControllerComponent::PredictBullet(const glm::vec2& origin, const glm::vec2& direction, float speed,
		int bouncesLeft, const glm::vec2& position, Threat& outThreat) const
	{
		if (speed <= 0.f || glm::dot(direction, direction) <= 0.f)
			return false;

		glm::vec2 point = origin;
		glm::vec2 heading = glm::normalize(direction);
		float travelled = 0.f;
		const float maxTravel = profile.threatHorizon * speed;

		while (travelled < maxTravel)
		{
			const glm::vec2 next = point + heading * predictionStep;

			if (DistanceToSegment(position, point, next) < hitRadius)
			{
				outThreat.impactPoint = next;
				outThreat.travelDirection = heading;
				outThreat.timeToImpact = travelled / speed;
				return true;
			}

			if (!NavGrid::Instance().IsWalkable(next))
			{
				if (bouncesLeft <= 0)
					return false;

				const bool blockedX = !NavGrid::Instance().IsWalkable({ next.x, point.y });
				const bool blockedY = !NavGrid::Instance().IsWalkable({ point.x, next.y });

				if (blockedX)
					heading.x = -heading.x;
				if (blockedY)
					heading.y = -heading.y;
				if (!blockedX && !blockedY)
					heading = -heading;

				--bouncesLeft;
				travelled += predictionStep;
				continue;
			}

			point = next;
			travelled += predictionStep;
		}

		return false;
	}

	glm::vec2 AITankControllerComponent::DodgeDestination(const glm::vec2& position, const Threat& threat) const
	{
		const glm::vec2 perpendicular{ -threat.travelDirection.y, threat.travelDirection.x };
		const float distance = 64.f;

		for (float sign : { 1.f, -1.f })
		{
			const glm::vec2 candidate = position + perpendicular * sign * distance;
			if (NavGrid::Instance().IsWalkable(candidate) && NavGrid::Instance().IsWalkable(position + perpendicular * sign * (distance * .5f)))
				return candidate;
		}

		for (float sign : { 1.f, -1.f })
		{
			const glm::vec2 candidate = position - threat.travelDirection * sign * distance;
			if (NavGrid::Instance().IsWalkable(candidate))
				return candidate;
		}

		return position;
	}

	void AITankControllerComponent::SampleTargetBehaviour(const glm::vec2& position, float deltaTime)
	{
		if (deltaTime <= 0.f)
			return;

		if (hasTarget)
		{
			const float range = glm::distance(position, targetPosition);
			if (lastRangeToTarget > 0.f)
			{
				const float closing = (lastRangeToTarget - range) / deltaTime;
				targetApproachRate += (closing - targetApproachRate) * std::min(1.f, deltaTime * 2.f);
			}
			lastRangeToTarget = range;
		}

		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (scene)
		{
			size_t bullets = 0;
			for (std::string_view tag : { Tags::Bullet, Tags::EnemyBullet })
				bullets += scene->FindGameObjectsByTag(tag).size();

			if (bullets > lastSeenBulletCount)
				targetFireRate += static_cast<float>(bullets - lastSeenBulletCount);

			lastSeenBulletCount = bullets;
		}

		targetFireRate = std::max(0.f, targetFireRate - deltaTime * .5f);
		damageMemory = std::max(0.f, damageMemory - deltaTime * .2f);

		if (auto* health = gameObject->GetComponent<HealthComponent>())
		{
			const int current = health->GetCurrentHealth();
			if (current < lastSeenHealth)
				damageMemory += 1.f;

			lastSeenHealth = current;
		}
	}

	void AITankControllerComponent::UpdateStance(const glm::vec2& position, float deltaTime)
	{
		decisionTimer -= deltaTime;
		if (decisionTimer > 0.f)
			return;

		decisionTimer = profile.decisionInterval;

		const float pressure = damageMemory + targetFireRate * .4f;
		const float tolerance = 2.f - profile.adaptationRate;
		const float range = hasTarget ? glm::distance(position, targetPosition) : 0.f;

		if (pressure > tolerance)
			stance = Stance::Hold;
		else if (targetApproachRate > 10.f && profile.adaptationRate > .4f && range < profile.preferredRange * 1.5f)
			stance = Stance::Flank;
		else
			stance = Stance::Press;
	}

	glm::vec2 AITankControllerComponent::ChooseGoal(const glm::vec2& position) const
	{
		if (!hasTarget)
			return position;

		const glm::vec2 away = position - targetPosition;
		glm::vec2 candidate = targetPosition;

		if (glm::dot(away, away) > 0.f)
			candidate = targetPosition + glm::normalize(away) * minEngageRange;

		if (stance == Stance::Hold)
		{
			if (glm::dot(away, away) > 0.f)
				candidate = targetPosition + glm::normalize(away) * profile.preferredRange;
		}
		else if (stance == Stance::Flank)
		{
			const glm::vec2 toTarget = targetPosition - position;
			if (glm::dot(toTarget, toTarget) > 0.f)
			{
				const glm::vec2 forward = glm::normalize(toTarget);
				const glm::vec2 side{ -forward.y, forward.x };
				const glm::vec2 left = targetPosition + side * flankOffset;
				const glm::vec2 right = targetPosition - side * flankOffset;

				if (NavGrid::Instance().IsWalkable(left) && glm::distance(position, left) <= glm::distance(position, right))
					candidate = left;
				else if (NavGrid::Instance().IsWalkable(right))
					candidate = right;
			}
		}

		return candidate;
	}

	void AITankControllerComponent::Replan(const glm::vec2& position, const glm::vec2& newGoal)
	{
		replanTimer = replanInterval;
		goal = newGoal;
		hasGoal = true;
		nextWaypoint = 0;

		AgentAvoidance::Instance().CollectObstacles(gameObject, obstacles);
		NavGrid::Instance().FindPath(position, goal, agentRadius, path, obstacles);
	}

	void AITankControllerComponent::StepAlongPath(const glm::vec2& position, float deltaTime)
	{
		while (nextWaypoint < path.size() && glm::distance(position, path[nextWaypoint]) < waypointRadius)
			++nextWaypoint;

		if (nextWaypoint >= path.size())
			return;

		MoveOneAxis(position, path[nextWaypoint], deltaTime);
	}

	void AITankControllerComponent::MoveOneAxis(const glm::vec2& position, const glm::vec2& desired, float deltaTime)
	{
		if (transform->IsMoving())
			return;

		const glm::vec2 delta = desired - position;
		if (glm::dot(delta, delta) <= 0.f)
			return;

		const AgentAvoidance::Verdict traffic = AgentAvoidance::Instance().Query(gameObject, glm::normalize(delta));

		const glm::vec2 axis = DominantAxis(delta);
		const float step = moveSpeed * traffic.speedScale * deltaTime;
		if (step <= 0.f)
			return;

		transform->SetPosition(transform->GetLocalPosition() + axis * step);
		transform->SetRotation(glm::degrees(std::atan2(-axis.y, axis.x)) - 90.f);
	}

	void AITankControllerComponent::UpdateAim(const glm::vec2& position, float deltaTime)
	{
		if (!turretAim || !hasTarget)
			return;

		glm::vec2 toTarget = targetPosition - position;
		const float range = glm::length(toTarget);
		if (range <= 0.f)
			return;

		const float flightTime = range / bulletSpeed;
		const glm::vec2 aimPoint = targetPosition + targetVelocity * flightTime * profile.leadPrediction;

		toTarget = aimPoint - position;
		if (glm::dot(toTarget, toTarget) <= 0.f)
			return;

		const float desired = NormalizeAngle(AimAngleFor(glm::normalize(toTarget)) + aimBias);
		const float difference = NormalizeAngle(desired - turretAim->GetAim());
		const float maxStep = profile.turretTurnRate * deltaTime;

		turretAim->Rotate(std::clamp(difference, -maxStep, maxStep));
	}

	void AITankControllerComponent::TryFire(const glm::vec2& position)
	{
		if (!shooting || !turretAim || !turret || !hasTarget)
			return;

		const glm::vec2 toTarget = targetPosition - position;
		if (glm::dot(toTarget, toTarget) <= 0.f)
			return;

		const float desired = AimAngleFor(glm::normalize(toTarget));
		if (std::abs(NormalizeAngle(desired - turretAim->GetAim())) > profile.fireTolerance)
			return;

		auto* turretTransform = turret->GetComponent<TransformComponent>();
		if (!turretTransform)
			return;

		const glm::vec2 forward = DirectionForAim(turretAim->GetAim());
		const glm::vec2 muzzle = turretTransform->GetWorldPosition() + turretTransform->GetPivot();

		const bool versus = GameData::CurrentGameMode == GameData::GameMode::Versus;

		if (!versus && EnemyPerception::BlockerInLineOfFire(gameObject, muzzle, forward, sightRange,
			[](std::string_view tag) { return tag == Tags::Player1; }))
			return;

		const auto hit = CollisionManager::Instance().RaycastFirstHit(muzzle, forward, sightRange, turret, gameObject);
		if (!hit || !hit->hitObject)
			return;

		const std::string_view hitTag = hit->hitObject->GetTag();
		if (!IsBulletTag(hitTag) && hitTag != teleportTag)
		{
			if (versus ? !Tags::IsPlayerTag(hitTag) : !Tags::IsEnemyTag(hitTag))
				return;
		}

		shooting->Shoot();
	}

	void AITankControllerComponent::Render(SDL_Renderer*)
	{
		if (!transform)
			return;

		const glm::vec2 position = transform->GetWorldPosition();

		DebugOverlay::Instance().SubmitFocusCandidate(gameObject, position, std::string("AI ") + DifficultyName(GameData::AiDifficulty));

		if (hasGoal)
		{
			const glm::vec2 toGoal = goal - position;
			if (glm::dot(toGoal, toGoal) > 0.f)
				AgentAvoidance::Instance().DebugRenderLane(gameObject, glm::normalize(toGoal));
		}

		if (DebugEnabled(DebugChannel::Paths))
			NavGrid::Instance().DebugRenderPath(path, nextWaypoint);

		if (!DebugOverlay::Instance().IsFocused(gameObject))
			return;

		const char* stanceName = stance == Stance::Press ? "PRESS" : (stance == Stance::Hold ? "HOLD" : "FLANK");

		DebugOverlay::Instance().FocusStat(gameObject, std::string("stance ") + stanceName);
		DebugOverlay::Instance().FocusStat(gameObject, "bullets " + std::to_string(threatCount));
		DebugOverlay::Instance().FocusStat(gameObject, dodging ? "dodging" : "steady");

		if (DebugEnabled(DebugChannel::AgentState))
			DebugDraw::DrawCircle(position + labelOffset, 3.f, { .4f, 1.f, .6f, 1.f });
	}
}
