#include "AITankControllerComponent.h"
#include "AgentAvoidance.h"
#include "BulletMoveComponent.h"
#include "Collider.h"
#include "DebugDraw.h"
#include "DebugOverlay.h"
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

		bool BoxesOverlap(const SDL_Rect& a, const SDL_Rect& b)
		{
			return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h;
		}

		bool EntryFace(const SDL_Rect& bullet, const SDL_Rect& wall, const glm::vec2& direction,
			glm::vec2& outNormal, float& outDepth)
		{
			const float lengthSquared = glm::dot(direction, direction);
			if (lengthSquared < 1e-8f)
				return false;

			const glm::vec2 unit = direction / std::sqrt(lengthSquared);

			const float bulletMin[2]{ static_cast<float>(bullet.x), static_cast<float>(bullet.y) };
			const float bulletMax[2]{ static_cast<float>(bullet.x + bullet.w), static_cast<float>(bullet.y + bullet.h) };
			const float wallMin[2]{ static_cast<float>(wall.x), static_cast<float>(wall.y) };
			const float wallMax[2]{ static_cast<float>(wall.x + wall.w), static_cast<float>(wall.y + wall.h) };

			int entryAxis = -1;
			float shortestTravel = 0.f;
			float entryDepth = 0.f;

			for (int axis = 0; axis < 2; ++axis)
			{
				if (std::abs(unit[axis]) < 1e-4f)
					continue;

				const float depth = unit[axis] > 0.f
					? bulletMax[axis] - wallMin[axis]
					: wallMax[axis] - bulletMin[axis];

				if (depth <= 0.f)
					continue;

				const float travel = depth / std::abs(unit[axis]);
				if (entryAxis < 0 || travel < shortestTravel)
				{
					entryAxis = axis;
					shortestTravel = travel;
					entryDepth = depth;
				}
			}

			if (entryAxis < 0)
				return false;

			outNormal = { 0.f, 0.f };
			outNormal[entryAxis] = unit[entryAxis] > 0.f ? -1.f : 1.f;
			outDepth = entryDepth;
			return true;
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

		if (!wallCacheBuilt)
			BuildWallCache();

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
		UpdateFiringSolution(position, deltaTime);
		UpdateAim(position, deltaTime);
		TryFire();
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

	void AITankControllerComponent::BuildWallCache()
	{
		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (!scene)
			return;

		wallRects.clear();
		wallBuckets.assign(static_cast<size_t>(bucketColumns) * bucketRows, {});

		for (GameObject* wall : scene->FindGameObjectsByTag(Tags::Wall))
		{
			if (auto* collider = wall->GetComponent<Collider>())
				wallRects.push_back(collider->GetBoundingBox());
		}

		for (int index = 0; index < static_cast<int>(wallRects.size()); ++index)
		{
			const SDL_Rect& rect = wallRects[static_cast<size_t>(index)];

			const int minX = std::clamp(rect.x / bucketSize, 0, bucketColumns - 1);
			const int minY = std::clamp(rect.y / bucketSize, 0, bucketRows - 1);
			const int maxX = std::clamp((rect.x + rect.w - 1) / bucketSize, 0, bucketColumns - 1);
			const int maxY = std::clamp((rect.y + rect.h - 1) / bucketSize, 0, bucketRows - 1);

			for (int y = minY; y <= maxY; ++y)
			{
				for (int x = minX; x <= maxX; ++x)
					wallBuckets[static_cast<size_t>(y) * bucketColumns + x].push_back(index);
			}
		}

		wallCacheBuilt = true;
	}

	bool AITankControllerComponent::FindWallBounce(const glm::vec2& point, const glm::vec2& direction,
		glm::vec2& outNormal, float& outDepth) const
	{
		const SDL_Rect bullet{
			static_cast<int>(point.x - bulletHalfWidth),
			static_cast<int>(point.y - bulletHalfHeight),
			bulletBoxWidth,
			bulletBoxHeight
		};

		const int minX = std::clamp(bullet.x / bucketSize, 0, bucketColumns - 1);
		const int minY = std::clamp(bullet.y / bucketSize, 0, bucketRows - 1);
		const int maxX = std::clamp((bullet.x + bullet.w) / bucketSize, 0, bucketColumns - 1);
		const int maxY = std::clamp((bullet.y + bullet.h) / bucketSize, 0, bucketRows - 1);

		int chosen = -1;

		for (int y = minY; y <= maxY; ++y)
		{
			for (int x = minX; x <= maxX; ++x)
			{
				for (int index : wallBuckets[static_cast<size_t>(y) * bucketColumns + x])
				{
					if (chosen >= 0 && index > chosen)
						continue;

					const SDL_Rect& wall = wallRects[static_cast<size_t>(index)];
					if (!BoxesOverlap(bullet, wall))
						continue;

					glm::vec2 normal{};
					float depth = 0.f;
					if (!EntryFace(bullet, wall, direction, normal, depth))
						continue;

					chosen = index;
					outNormal = normal;
					outDepth = depth;
				}
			}
		}

		return chosen >= 0;
	}

	glm::vec2 AITankControllerComponent::MuzzlePoint(const glm::vec2& forward) const
	{
		if (!turret)
			return { 0.f, 0.f };

		auto* turretTransform = turret->GetComponent<TransformComponent>();
		if (!turretTransform)
			return { 0.f, 0.f };

		return turretTransform->GetWorldPosition() + turretTransform->GetPivot() + forward * muzzleOffset;
	}

	AITankControllerComponent::ShotResult AITankControllerComponent::SimulateShot(const glm::vec2& origin,
		const glm::vec2& direction, int maxBounces, std::vector<glm::vec2>* outPath) const
	{
		ShotResult result{};

		if (outPath)
		{
			outPath->clear();
			outPath->push_back(origin);
		}

		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (!scene || !wallCacheBuilt || glm::dot(direction, direction) <= 0.f)
			return result;

		struct TankBox
		{
			SDL_Rect box;
			ShotOutcome outcome;
		};

		std::vector<TankBox> tanks;
		SDL_Rect selfBox{ 0, 0, 0, 0 };

		const auto boxOf = [](GameObject* object, SDL_Rect& out)
			{
				auto* collider = object ? object->GetComponent<Collider>() : nullptr;
				if (!collider)
					return false;

				out = collider->GetBoundingBox();
				return true;
			};

		if (!boxOf(gameObject, selfBox))
			return result;

		const bool versus = GameData::CurrentGameMode == GameData::GameMode::Versus;

		if (versus)
		{
			if (GameObject* human = scene->FindGameObjectByTag(std::string(Tags::Player1)))
			{
				SDL_Rect box{};
				if (boxOf(human, box))
					tanks.push_back({ box, ShotOutcome::HitTarget });
			}
		}
		else
		{
			if (GameObject* ally = scene->FindGameObjectByTag(std::string(Tags::Player1)))
			{
				SDL_Rect box{};
				if (boxOf(ally, box))
					tanks.push_back({ box, ShotOutcome::HitAlly });
			}

			for (std::string_view tag : { Tags::BlueTank, Tags::PinkTank, Tags::Recognizer })
			{
				for (GameObject* enemy : scene->FindGameObjectsByTag(tag))
				{
					SDL_Rect box{};
					if (boxOf(enemy, box))
						tanks.push_back({ box, ShotOutcome::HitTarget });
				}
			}
		}

		SDL_Rect teleportBox{ 0, 0, 0, 0 };
		const bool hasTeleport = boxOf(scene->FindGameObjectByTag(std::string(teleportTag)), teleportBox);

		glm::vec2 point = origin;
		glm::vec2 heading = glm::normalize(direction);
		float travelled = 0.f;
		int bounces = 0;
		bool leftSelf = false;

		while (travelled < simMaxPath)
		{
			point += heading * simStep;
			travelled += simStep;

			const SDL_Rect bulletBox{
				static_cast<int>(point.x - bulletHalfWidth),
				static_cast<int>(point.y - bulletHalfHeight),
				bulletBoxWidth,
				bulletBoxHeight
			};

			if (!leftSelf && !BoxesOverlap(bulletBox, selfBox))
				leftSelf = true;

			if (hasTeleport && BoxesOverlap(bulletBox, teleportBox))
				break;

			const float margin = std::min(selfMarginMax, (travelled / bulletSpeed) * moveSpeed * selfMarginScale);
			const SDL_Rect riskBox{
				selfBox.x - static_cast<int>(margin),
				selfBox.y - static_cast<int>(margin),
				selfBox.w + static_cast<int>(margin) * 2,
				selfBox.h + static_cast<int>(margin) * 2
			};

			if (leftSelf && BoxesOverlap(bulletBox, riskBox))
			{
				result.outcome = ShotOutcome::HitSelf;
				result.bounces = bounces;
				result.pathLength = travelled;
				if (outPath)
					outPath->push_back(point);

				return result;
			}

			bool struckTank = false;
			for (const TankBox& tank : tanks)
			{
				if (!BoxesOverlap(bulletBox, tank.box))
					continue;

				result.outcome = tank.outcome;
				result.bounces = bounces;
				result.pathLength = travelled;
				struckTank = true;
				break;
			}

			if (struckTank)
			{
				if (outPath)
					outPath->push_back(point);

				return result;
			}

			glm::vec2 normal{};
			float depth = 0.f;
			if (FindWallBounce(point, heading, normal, depth))
			{
				if (bounces >= maxBounces)
					break;

				point += normal * (depth + separationBias);
				heading = glm::normalize(glm::reflect(heading, normal));
				++bounces;

				if (outPath)
					outPath->push_back(point);
			}
		}

		result.bounces = bounces;
		result.pathLength = travelled;

		if (outPath)
			outPath->push_back(point);

		return result;
	}

	float AITankControllerComponent::DirectAimAngle(const glm::vec2& position, float flightTime) const
	{
		const glm::vec2 aimPoint = targetPosition + targetVelocity * flightTime * profile.leadPrediction;
		const glm::vec2 toAim = aimPoint - position;

		if (glm::dot(toAim, toAim) <= 0.f)
			return turretAim ? turretAim->GetAim() : 0.f;

		return AimAngleFor(glm::normalize(toAim));
	}

	float AITankControllerComponent::AimTargetAngle() const
	{
		return NormalizeAngle(solution.aimAngle + aimBias);
	}

	void AITankControllerComponent::UpdateFiringSolution(const glm::vec2& position, float deltaTime)
	{
		solutionTimer -= deltaTime;

		if (!hasTarget || !turretAim)
		{
			solution.valid = false;
			return;
		}

		const float range = glm::distance(position, targetPosition);
		const float directAngle = DirectAimAngle(position, range / bulletSpeed);

		const glm::vec2 directHeading = DirectionForAim(directAngle);
		lastDirectOutcome = SimulateShot(MuzzlePoint(directHeading), directHeading, bulletMaxBounces).outcome;
		if (lastDirectOutcome == ShotOutcome::HitTarget)
		{
			solution = { directAngle, 0, true };
			return;
		}

		if (solution.valid)
		{
			const glm::vec2 heldHeading = DirectionForAim(solution.aimAngle);
			if (SimulateShot(MuzzlePoint(heldHeading), heldHeading, bulletMaxBounces).outcome == ShotOutcome::HitTarget)
				return;
		}

		if (solutionTimer > 0.f)
			return;

		solutionTimer = profile.bankInterval;

		FiringSolution best{};
		float bestPath = 0.f;

		for (float angle = -180.f; angle < 180.f; angle += profile.bankSearchStep)
		{
			const glm::vec2 heading = DirectionForAim(angle);
			const ShotResult shot = SimulateShot(MuzzlePoint(heading), heading, profile.bankBounces);

			if (shot.outcome != ShotOutcome::HitTarget)
				continue;

			if (!best.valid || shot.pathLength < bestPath)
			{
				best = { angle, shot.bounces, true };
				bestPath = shot.pathLength;
			}
		}

		solution = best;
	}

	void AITankControllerComponent::UpdateAim(const glm::vec2& position, float deltaTime)
	{
		if (!turretAim || !hasTarget)
			return;

		const float desired = solution.valid
			? AimTargetAngle()
			: NormalizeAngle(DirectAimAngle(position, glm::distance(position, targetPosition) / bulletSpeed) + aimBias);

		const float difference = NormalizeAngle(desired - turretAim->GetAim());
		const float maxStep = profile.turretTurnRate * deltaTime;

		turretAim->Rotate(std::clamp(difference, -maxStep, maxStep));
	}

	bool AITankControllerComponent::BarrelShot(glm::vec2& outMuzzle, glm::vec2& outForward) const
	{
		auto* turretTransform = turret ? turret->GetComponent<TransformComponent>() : nullptr;
		if (!turretTransform)
			return false;

		const float barrelRadians = glm::radians(turretTransform->GetWorldRotation() - 90.f);
		outForward = { std::cos(barrelRadians), std::sin(barrelRadians) };
		outMuzzle = turretTransform->GetWorldPosition() + turretTransform->GetPivot() + outForward * muzzleOffset;
		return true;
	}

	void AITankControllerComponent::TryFire()
	{
		if (!shooting || !turretAim || !turret || !hasTarget || !solution.valid)
			return;

		glm::vec2 muzzle{};
		glm::vec2 forward{};
		if (!BarrelShot(muzzle, forward))
			return;

		if (std::abs(NormalizeAngle(AimTargetAngle() - AimAngleFor(forward))) > profile.fireTolerance)
			return;

		const ShotResult shot = SimulateShot(muzzle, forward, bulletMaxBounces);

		if (shot.outcome == ShotOutcome::HitSelf || shot.outcome == ShotOutcome::HitAlly)
		{
			++selfBlockedCount;
			return;
		}

		shooting->Shoot();
	}

	void AITankControllerComponent::RenderPrediction()
	{
		glm::vec2 muzzle{};
		glm::vec2 forward{};
		if (!BarrelShot(muzzle, forward))
			return;

		const ShotResult shot = SimulateShot(muzzle, forward, bulletMaxBounces, &debugPath);
		if (debugPath.size() < 2)
			return;

		glm::vec4 color{ .55f, .65f, .85f, .65f };
		if (shot.outcome == ShotOutcome::HitTarget)
			color = { .3f, 1.f, .45f, 1.f };
		else if (shot.outcome == ShotOutcome::HitSelf)
			color = { 1.f, .2f, .2f, 1.f };
		else if (shot.outcome == ShotOutcome::HitAlly)
			color = { 1.f, .55f, .1f, 1.f };

		for (size_t i = 1; i < debugPath.size(); ++i)
			DebugDraw::DrawLine(debugPath[i - 1], debugPath[i], color);

		DebugDraw::DrawCircle(debugPath.front(), 3.f, color);

		for (size_t i = 1; i + 1 < debugPath.size(); ++i)
			DebugDraw::DrawCircle(debugPath[i], 5.f, color);

		DebugDraw::DrawCircle(debugPath.back(), 8.f, color);
	}

	void AITankControllerComponent::Render(SDL_Renderer*)
	{
		if (!transform)
			return;

		if (DebugEnabled(DebugChannel::Prediction))
			RenderPrediction();

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

		const char* directName =
			lastDirectOutcome == ShotOutcome::HitTarget ? "target" :
			lastDirectOutcome == ShotOutcome::HitSelf ? "self" :
			lastDirectOutcome == ShotOutcome::HitAlly ? "ally" : "miss";

		DebugOverlay::Instance().FocusStat(gameObject, std::string("direct ") + directName);
		DebugOverlay::Instance().FocusStat(gameObject, "selfblocked " + std::to_string(selfBlockedCount));

		if (!solution.valid)
			DebugOverlay::Instance().FocusStat(gameObject, "shot none");
		else if (solution.bounces == 0)
			DebugOverlay::Instance().FocusStat(gameObject, "shot direct");
		else
			DebugOverlay::Instance().FocusStat(gameObject, "shot bank x" + std::to_string(solution.bounces));

		if (DebugEnabled(DebugChannel::AgentState))
			DebugDraw::DrawCircle(position + labelOffset, 3.f, { .4f, 1.f, .6f, 1.f });
	}
}
