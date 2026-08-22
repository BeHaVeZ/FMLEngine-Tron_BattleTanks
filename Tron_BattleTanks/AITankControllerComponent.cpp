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

		glm::vec2 SecondaryAxis(const glm::vec2& direction)
		{
			if (std::abs(direction.x) >= std::abs(direction.y))
				return { 0.f, direction.y > 0.f ? 1.f : (direction.y < 0.f ? -1.f : 0.f) };

			return { direction.x > 0.f ? 1.f : (direction.x < 0.f ? -1.f : 0.f), 0.f };
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

		RollAimBias();
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

		GatherSceneObjects();

		const glm::vec2 position = transform->GetWorldPosition();

		if (glm::distance(position, lastPosition) > teleportThreshold)
		{
			path.clear();
			nextWaypoint = 0;
			hasGoal = false;
			dodging = false;
			heldMoveAxisValid = false;
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
				if (target == targetObject.Get() && glm::distance(seenAt, lastTargetPosition) <= teleportThreshold)
					targetVelocity = (seenAt - lastTargetPosition) / std::max(profile.reactionDelay, .016f);
				else
					targetVelocity = { 0.f, 0.f };

				lastTargetPosition = seenAt;
				targetPosition = seenAt;
				targetObject = target;

				auto* targetHealth = target->GetComponent<HealthComponent>();
				targetInvulnerable = targetHealth && targetHealth->IsInInvulnerabilityWindow();
			}
			else
			{
				targetInvulnerable = false;
				targetObject = {};
				targetVelocity = { 0.f, 0.f };
			}
		}
		else if (HasTarget())
		{
			targetPosition += LeadVelocity() * deltaTime;
		}

		SampleTargetBehaviour(position, deltaTime);

		fireDelayTimer -= deltaTime;

		Threat threat{};
		const bool threatened = ScanForThreats(position, threat);

		dodgeHold -= deltaTime;
		if (threatened && threat.timeToImpact <= dodgeReactWindow)
		{
			++threatSeenCount;

			if (dodging)
			{
				dodgeHold = dodgeHoldTime * profile.dodgeCommitment;
			}
			else
			{
				int axisIndex = -1;
				if (!EscapeDirection(position, threat, dodgeBlockedMask, preferredDodgeAxis, axisIndex))
				{
					++escapeFailCount;
				}
				else
				{
					++dodgeCount;

					if (preferredDodgeAxis >= 0 && axisIndex == (preferredDodgeAxis ^ 1))
						++dodgeReversalCount;

					dodgeAxisIndex = axisIndex;
					preferredDodgeAxis = axisIndex;
					dodgeAxis = EscapeAxis(axisIndex);
					dodgeTarget = position + dodgeAxis * escapeProbe;
					dodgeProgressPosition = position;
					dodgeStuckTimer = 0.f;
					heldMoveAxisValid = false;
					axisHoldTimer = 0.f;
					dodgeHold = dodgeHoldTime * profile.dodgeCommitment;
					dodging = true;
				}
			}
		}
		else if (dodgeHold <= 0.f)
		{
			dodging = false;
			dodgeAxisIndex = -1;
			preferredDodgeAxis = -1;
			dodgeBlockedMask = 0;
		}

		if (dodging)
		{
			dodgeStuckTimer += deltaTime;
			if (dodgeStuckTimer >= dodgeStuckWindow)
			{
				const bool stalled = glm::distance(position, dodgeProgressPosition) < dodgeStuckDistance;
				dodgeProgressPosition = position;
				dodgeStuckTimer = 0.f;

				if (stalled)
				{
					++dodgeStallCount;

					if (dodgeAxisIndex >= 0)
						dodgeBlockedMask |= 1 << dodgeAxisIndex;

					dodging = false;
					dodgeAxisIndex = -1;
				}
			}

			if (dodging && glm::distance(position, dodgeTarget) < waypointRadius)
			{
				dodging = false;
				dodgeAxisIndex = -1;
			}
		}

		UpdateStance(position, deltaTime);

		progressTimer += deltaTime;
		if (progressTimer >= progressWindow)
		{
			const bool shouldBeMoving = dodging || nextWaypoint < path.size();
			const bool stalled = shouldBeMoving
				&& intendedTravel >= progressDistance * 2.f
				&& glm::distance(position, progressPosition) < progressDistance;

			progressPosition = position;
			progressTimer = 0.f;
			intendedTravel = 0.f;

			if (stalled)
			{
				++movementStallCount;

				path.clear();
				nextWaypoint = 0;
				hasGoal = false;
				replanTimer = 0.f;

				strafeSign = -strafeSign;
				strafeTimer = strafeFlipInterval;

				if (dodgeAxisIndex >= 0)
					dodgeBlockedMask |= 1 << dodgeAxisIndex;

				dodging = false;
				dodgeAxisIndex = -1;
				heldMoveAxisValid = false;
			}
		}

		const glm::vec2 desiredGoal = ChooseGoal(position);

		replanTimer -= deltaTime;
		const bool goalMoved = !hasGoal || glm::distance(desiredGoal, goal) > goalMovedThreshold;
		const bool pathDone = nextWaypoint >= path.size();

		if ((goalMoved || pathDone) && (replanTimer <= 0.f || pathDone))
			Replan(position, desiredGoal);

		strafeTimer -= deltaTime;
		if (strafeTimer <= 0.f)
		{
			strafeTimer = strafeFlipInterval;
			strafeSign = -strafeSign;
		}

		if (dodging)
			MoveOneAxis(position, dodgeTarget, deltaTime);
		else if (!TryKite(position, deltaTime) && !TryStrafe(position, deltaTime))
			StepAlongPath(position, deltaTime);

		GatherShotContext();
		UpdateFiringSolution(position, deltaTime);
		UpdateAim(position, deltaTime);
		TryFire();
	}

	void AITankControllerComponent::GatherSceneObjects()
	{
		scenePlayer1 = nullptr;
		sceneTeleport = nullptr;
		sceneEnemies.clear();
		sceneBullets.clear();

		Scene* scene = SceneManager::Instance().GetCurrentScene();
		if (!scene)
			return;

		scene->ForEachGameObject([this](GameObject& object)
			{
				const std::string& tag = object.GetTag();
				if (tag == Tags::Player1)
					scenePlayer1 = &object;
				else if (Tags::IsEnemyTag(tag))
					sceneEnemies.push_back(&object);
				else if (tag == Tags::Bullet || tag == Tags::EnemyBullet)
					sceneBullets.push_back(&object);
				else if (tag == teleportTag)
					sceneTeleport = &object;
			});
	}

	GameObject* AITankControllerComponent::AcquireTarget() const
	{
		if (GameData::CurrentGameMode == GameData::GameMode::Versus)
			return scenePlayer1;

		const glm::vec2 position = transform->GetWorldPosition();
		GameObject* closest = nullptr;
		float closestDistance = 0.f;

		for (GameObject* enemy : sceneEnemies)
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

		return closest;
	}

	bool AITankControllerComponent::ScanForThreats(const glm::vec2& position, Threat& outThreat)
	{
		bool found = false;
		Threat nearest{};
		int seen = 0;

		for (GameObject* bullet : sceneBullets)
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

		threatCount = seen;

		if (found)
			outThreat = nearest;

		return found;
	}

	bool AITankControllerComponent::PredictBullet(const glm::vec2& origin, const glm::vec2& direction, float speed,
		int bouncesLeft, const glm::vec2& position, Threat& outThreat) const
	{
		if (speed <= 0.f || glm::dot(direction, direction) <= 0.f || !wallCacheBuilt)
			return false;

		glm::vec2 point = origin;
		glm::vec2 heading = glm::normalize(direction);
		float travelled = 0.f;
		const float maxTravel = profile.threatHorizon * speed;

		while (travelled < maxTravel)
		{
			const glm::vec2 previous = point;
			const int bouncesBefore = bouncesLeft;

			if (!StepBullet(point, heading, bouncesLeft))
				return false;

			travelled += simStep;

			const bool bounced = bouncesLeft != bouncesBefore;
			const float distance = bounced
				? glm::distance(position, point)
				: DistanceToSegment(position, previous, point);

			if (distance < hitRadius)
			{
				outThreat.impactPoint = point;
				outThreat.travelDirection = heading;
				outThreat.timeToImpact = travelled / speed;
				return true;
			}
		}

		return false;
	}

	glm::vec2 AITankControllerComponent::EscapeAxis(int index)
	{
		switch (index)
		{
		case 0:
			return { 1.f, 0.f };
		case 1:
			return { -1.f, 0.f };
		case 2:
			return { 0.f, 1.f };
		default:
			return { 0.f, -1.f };
		}
	}

	bool AITankControllerComponent::PathIsClear(const glm::vec2& from, const glm::vec2& axis, float distance) const
	{
		constexpr int samples = 4;

		const glm::vec2 side{ -axis.y, axis.x };
		auto& grid = NavGrid::Instance();

		for (int i = 0; i <= samples; ++i)
		{
			const float travelled = agentRadius + distance * static_cast<float>(i) / samples;
			const glm::vec2 centre = from + axis * travelled;

			if (!grid.IsWalkable(centre) || !grid.IsWalkable(centre + side * bodyProbe) || !grid.IsWalkable(centre - side * bodyProbe))
				return false;
		}

		return true;
	}

	bool AITankControllerComponent::EscapeDirection(const glm::vec2& position, const Threat& threat, int blockedMask, int preferredIndex, int& outIndex) const
	{
		const float reach = std::min(escapeProbe, moveSpeed * std::max(threat.timeToImpact, .05f));
		const glm::vec2 lineStart = threat.impactPoint - threat.travelDirection * 600.f;
		const glm::vec2 lineEnd = threat.impactPoint + threat.travelDirection * 600.f;

		bool found = false;
		float bestScore = 0.f;

		for (int index = 0; index < escapeAxisCount; ++index)
		{
			if (blockedMask & (1 << index))
				continue;

			const glm::vec2 axis = EscapeAxis(index);
			if (!PathIsClear(position, axis, escapeProbe))
				continue;

			const glm::vec2 destination = position + axis * reach;
			const float clearance = DistanceToSegment(destination, lineStart, lineEnd);
			const float lateral = 1.f - std::abs(glm::dot(axis, threat.travelDirection));
			const float score = clearance + lateral * escapeClearance + (index == preferredIndex ? dodgeStickiness : 0.f);

			if (!found || score > bestScore)
			{
				bestScore = score;
				outIndex = index;
				found = true;
			}
		}

		return found;
	}

	bool AITankControllerComponent::TryKite(const glm::vec2& position, float deltaTime)
	{
		const bool wasKiting = kiting;
		kiting = false;

		if (GameData::CurrentGameMode != GameData::GameMode::Coop)
			return false;

		glm::vec2 nearest{ 0.f, 0.f };
		float nearestDistance = kiteRadius;
		bool found = false;

		for (GameObject* recognizer : sceneEnemies)
		{
			if (recognizer->GetTag() != Tags::Recognizer)
				continue;

			auto* recognizerTransform = recognizer->GetComponent<TransformComponent>();
			if (!recognizerTransform)
				continue;

			const glm::vec2 at = recognizerTransform->GetWorldPosition();
			const float distance = glm::distance(position, at);
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearest = at;
				found = true;
			}
		}

		if (!found)
			return false;

		const glm::vec2 away = position - nearest;
		if (glm::dot(away, away) <= 0.f)
			return false;

		const glm::vec2 retreat = glm::normalize(away);

		int bestIndex = -1;
		float bestScore = 0.f;

		for (int index = 0; index < escapeAxisCount; ++index)
		{
			const glm::vec2 axis = EscapeAxis(index);
			const float score = glm::dot(axis, retreat);

			if (score <= 0.f || !PathIsClear(position, axis, strafeProbe))
				continue;

			if (bestIndex < 0 || score > bestScore)
			{
				bestScore = score;
				bestIndex = index;
			}
		}

		if (bestIndex < 0)
			return false;

		kiting = true;
		if (!wasKiting)
			++kiteCount;

		MoveOneAxis(position, position + EscapeAxis(bestIndex) * strafeProbe, deltaTime);
		return true;
	}

	bool AITankControllerComponent::TryStrafe(const glm::vec2& position, float deltaTime)
	{
		if (profile.strafeAmount <= 0.f || !HasTarget() || !solution.valid)
			return false;

		const float range = glm::distance(position, targetPosition);
		if (range > profile.engageRange * 1.4f || range < profile.engageRange * .6f)
			return false;

		if (strafeTimer < strafeFlipInterval * (1.f - profile.strafeAmount))
			return true;

		const glm::vec2 toTarget = targetPosition - position;
		if (glm::dot(toTarget, toTarget) <= 0.f)
			return false;

		const glm::vec2 forward = glm::normalize(toTarget);
		glm::vec2 axis = DominantAxis(glm::vec2{ -forward.y, forward.x } * strafeSign);

		if (!PathIsClear(position, axis, strafeProbe))
		{
			strafeSign = -strafeSign;
			axis = -axis;

			if (!PathIsClear(position, axis, strafeProbe))
				return false;
		}

		MoveOneAxis(position, position + axis * strafeProbe, deltaTime);
		return true;
	}

	void AITankControllerComponent::SampleTargetBehaviour(const glm::vec2& position, float deltaTime)
	{
		if (deltaTime <= 0.f)
			return;

		if (HasTarget())
		{
			const float range = glm::distance(position, targetPosition);
			if (lastRangeToTarget > 0.f)
			{
				const float closing = (lastRangeToTarget - range) / deltaTime;
				targetApproachRate += (closing - targetApproachRate) * std::min(1.f, deltaTime * 2.f);
			}
			lastRangeToTarget = range;
		}

		{
			const size_t bullets = sceneBullets.size();

			if (bullets > lastSeenBulletCount)
			{
				const int appeared = static_cast<int>(bullets - lastSeenBulletCount);
				const int ownShots = std::min(appeared, pendingOwnShots);
				pendingOwnShots -= ownShots;
				targetFireRate += static_cast<float>(appeared - ownShots);
			}

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
		const float range = HasTarget() ? glm::distance(position, targetPosition) : 0.f;

		if (pressure > tolerance)
			stance = Stance::Hold;
		else if (targetApproachRate > 10.f && profile.adaptationRate > .4f && range < profile.preferredRange * 1.5f)
			stance = Stance::Flank;
		else
			stance = Stance::Press;
	}

	glm::vec2 AITankControllerComponent::ChooseGoal(const glm::vec2& position) const
	{
		if (!HasTarget())
			return position;

		const glm::vec2 away = position - targetPosition;
		glm::vec2 candidate = targetPosition;

		if (glm::dot(away, away) > 0.f)
			candidate = targetPosition + glm::normalize(away) * profile.engageRange;

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

		const float intendedStep = moveSpeed * deltaTime;
		intendedTravel += intendedStep;

		const float step = intendedStep * traffic.speedScale;
		if (step <= 0.f)
			return;

		axisHoldTimer -= deltaTime;

		glm::vec2 axis{ 0.f, 0.f };
		const bool heldClear = heldMoveAxisValid && PathIsClear(position, heldMoveAxis, moveProbe);
		const bool heldProductive = heldMoveAxisValid && glm::dot(delta, heldMoveAxis) >= axisReleaseDistance;

		if (heldClear && (heldProductive || axisHoldTimer > 0.f))
		{
			axis = heldMoveAxis;
		}
		else
		{
			axis = DominantAxis(delta);
			if (!PathIsClear(position, axis, moveProbe))
			{
				const glm::vec2 alternate = SecondaryAxis(delta);
				if (glm::dot(alternate, alternate) > 0.f && PathIsClear(position, alternate, moveProbe))
					axis = alternate;
			}

			if (heldMoveAxisValid && (axis.x != heldMoveAxis.x || axis.y != heldMoveAxis.y))
				++axisFlipCount;

			heldMoveAxis = axis;
			heldMoveAxisValid = true;
			axisHoldTimer = minAxisHold;
		}

		transform->SetPosition(transform->GetLocalPosition() + axis * step);
		transform->MarkMoving(true);
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

	bool AITankControllerComponent::StepBullet(glm::vec2& point, glm::vec2& heading, int& bouncesLeft) const
	{
		point += heading * simStep;

		glm::vec2 normal{};
		float depth = 0.f;
		if (!FindWallBounce(point, heading, normal, depth))
			return true;

		if (bouncesLeft <= 0)
			return false;

		point += normal * (depth + separationBias);
		heading = glm::normalize(glm::reflect(heading, normal));
		--bouncesLeft;
		return true;
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

	void AITankControllerComponent::GatherShotContext()
	{
		shotTanks.clear();
		shotSelfValid = false;
		shotTeleportValid = false;

		const auto boxOf = [](GameObject* object, SDL_Rect& out)
			{
				auto* collider = object ? object->GetComponent<Collider>() : nullptr;
				if (!collider)
					return false;

				out = collider->GetBoundingBox();
				return true;
			};

		const auto addTank = [&](GameObject* object, ShotOutcome outcome)
			{
				SDL_Rect box{};
				if (!boxOf(object, box))
					return;

				TankBox tank{ box, outcome };
				auto* objectTransform = object == targetObject.Get() ? object->GetComponent<TransformComponent>() : nullptr;
				if (objectTransform)
				{
					tank.offset = targetPosition - objectTransform->GetWorldPosition();
					tank.velocity = LeadVelocity();
				}

				shotTanks.push_back(tank);
			};

		shotSelfValid = boxOf(gameObject, shotSelfBox);
		shotTeleportValid = boxOf(sceneTeleport, shotTeleportBox);

		if (GameData::CurrentGameMode == GameData::GameMode::Versus)
		{
			addTank(scenePlayer1, ShotOutcome::HitTarget);
			return;
		}

		addTank(scenePlayer1, ShotOutcome::HitAlly);

		for (GameObject* enemy : sceneEnemies)
			addTank(enemy, ShotOutcome::HitTarget);
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

		if (!wallCacheBuilt || !shotSelfValid || glm::dot(direction, direction) <= 0.f)
			return result;

		glm::vec2 point = origin;
		glm::vec2 heading = glm::normalize(direction);
		float travelled = 0.f;
		int bouncesLeft = maxBounces;
		int bounces = 0;
		bool leftSelf = false;

		while (travelled < simMaxPath)
		{
			const int bouncesBefore = bouncesLeft;
			if (!StepBullet(point, heading, bouncesLeft))
				break;

			travelled += simStep;

			if (bouncesLeft != bouncesBefore)
			{
				++bounces;
				if (outPath)
					outPath->push_back(point);
			}

			const SDL_Rect bulletBox{
				static_cast<int>(point.x - bulletHalfWidth),
				static_cast<int>(point.y - bulletHalfHeight),
				bulletBoxWidth,
				bulletBoxHeight
			};

			if (!leftSelf && !BoxesOverlap(bulletBox, shotSelfBox))
				leftSelf = true;

			if (shotTeleportValid && BoxesOverlap(bulletBox, shotTeleportBox))
				break;

			const float margin = std::min(selfMarginMax, (travelled / bulletSpeed) * moveSpeed * selfMarginScale);
			const SDL_Rect riskBox{
				shotSelfBox.x - static_cast<int>(margin),
				shotSelfBox.y - static_cast<int>(margin),
				shotSelfBox.w + static_cast<int>(margin) * 2,
				shotSelfBox.h + static_cast<int>(margin) * 2
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

			const float flightTime = travelled / bulletSpeed;
			bool struckTank = false;
			for (const TankBox& tank : shotTanks)
			{
				const glm::vec2 shift = tank.offset + tank.velocity * flightTime;
				const SDL_Rect shifted{
					tank.box.x + static_cast<int>(shift.x),
					tank.box.y + static_cast<int>(shift.y),
					tank.box.w,
					tank.box.h
				};

				if (!BoxesOverlap(bulletBox, shifted))
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

		}

		result.bounces = bounces;
		result.pathLength = travelled;

		if (outPath)
			outPath->push_back(point);

		return result;
	}

	glm::vec2 AITankControllerComponent::LeadVelocity() const
	{
		return targetVelocity * profile.leadPrediction * leadBias;
	}

	glm::vec2 AITankControllerComponent::PredictedTargetPosition(float time) const
	{
		return targetPosition + LeadVelocity() * time;
	}

	float AITankControllerComponent::InterceptTime(const glm::vec2& position) const
	{
		const auto flightTime = [&](const glm::vec2& point)
			{
				return std::max(0.f, glm::distance(position, point) - muzzleOffset) / bulletSpeed;
			};

		float time = flightTime(targetPosition);
		for (int i = 0; i < interceptIterations; ++i)
			time = flightTime(PredictedTargetPosition(time));

		return time;
	}

	float AITankControllerComponent::DirectAimAngle(const glm::vec2& position) const
	{
		const glm::vec2 aimPoint = PredictedTargetPosition(InterceptTime(position));
		const glm::vec2 toAim = aimPoint - position;

		if (glm::dot(toAim, toAim) <= 0.f)
			return turretAim ? turretAim->GetAim() : 0.f;

		return AimAngleFor(glm::normalize(toAim));
	}

	float AITankControllerComponent::AimTargetAngle() const
	{
		return NormalizeAngle(solution.aimAngle + aimBias);
	}

	void AITankControllerComponent::RollAimBias()
	{
		std::uniform_real_distribution<float> spread(-profile.aimError, profile.aimError);
		aimBias = spread(rng);

		std::uniform_real_distribution<float> lead(std::max(0.f, 1.f - profile.leadError), 1.f + profile.leadError);
		leadBias = lead(rng);
	}

	void AITankControllerComponent::UpdateFiringSolution(const glm::vec2& position, float deltaTime)
	{
		solutionTimer -= deltaTime;

		if (!HasTarget() || !turretAim)
		{
			solution.valid = false;
			return;
		}

		const float directAngle = DirectAimAngle(position);

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

		if (!bankScanActive)
		{
			if (solutionTimer > 0.f)
				return;

			solutionTimer = profile.bankInterval;
			bankScanActive = true;
			bankScanAngle = -180.f;
			bankBest = {};
			bankBestPath = 0.f;
		}

		for (int processed = 0; bankScanActive && processed < bankAnglesPerFrame; ++processed)
		{
			const glm::vec2 heading = DirectionForAim(bankScanAngle);
			const ShotResult shot = SimulateShot(MuzzlePoint(heading), heading, profile.bankBounces);

			if (shot.outcome == ShotOutcome::HitTarget && (!bankBest.valid || shot.pathLength < bankBestPath))
			{
				bankBest = { bankScanAngle, shot.bounces, true };
				bankBestPath = shot.pathLength;
			}

			bankScanAngle += profile.bankSearchStep;
			if (bankScanAngle >= 180.f)
			{
				bankScanActive = false;
				solution = bankBest;
			}
		}
	}

	void AITankControllerComponent::UpdateAim(const glm::vec2& position, float deltaTime)
	{
		if (!turretAim || !HasTarget())
			return;

		const float desired = solution.valid
			? AimTargetAngle()
			: NormalizeAngle(DirectAimAngle(position) + aimBias);

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
		if (!shooting || !turretAim || !turret || !HasTarget() || !solution.valid)
			return;

		if (fireDelayTimer > 0.f || targetInvulnerable)
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
		++pendingOwnShots;
		fireDelayTimer = profile.fireDelay;
		RollAimBias();
	}

	void AITankControllerComponent::RenderPrediction()
	{
		if (HasTarget() && transform)
		{
			const glm::vec2 aimPoint = PredictedTargetPosition(InterceptTime(transform->GetWorldPosition()));
			const glm::vec4 leadColor{ 1.f, .9f, .3f, .9f };
			DebugDraw::DrawLine(targetPosition, aimPoint, leadColor);
			DebugDraw::DrawCircle(targetPosition, 4.f, leadColor);
			DebugDraw::DrawCircle(aimPoint, 6.f, leadColor);
		}

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
		if (!transform || !DebugOverlay::Instance().IsMasterEnabled())
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
		DebugOverlay::Instance().FocusStat(gameObject, dodging ? "dodging" : (kiting ? "kiting" : "steady"));

		const char* directName =
			lastDirectOutcome == ShotOutcome::HitTarget ? "target" :
			lastDirectOutcome == ShotOutcome::HitSelf ? "self" :
			lastDirectOutcome == ShotOutcome::HitAlly ? "ally" : "miss";

		DebugOverlay::Instance().FocusStat(gameObject, std::string("direct ") + directName);
		DebugOverlay::Instance().FocusStat(gameObject, "selfblocked " + std::to_string(selfBlockedCount));
		DebugOverlay::Instance().FocusStat(gameObject, "threats " + std::to_string(threatSeenCount)
			+ " dodges " + std::to_string(dodgeCount)
			+ " stalls " + std::to_string(dodgeStallCount)
			+ " noesc " + std::to_string(escapeFailCount)
			+ " mstall " + std::to_string(movementStallCount)
			+ " rev " + std::to_string(dodgeReversalCount)
			+ " flips " + std::to_string(axisFlipCount)
			+ " kites " + std::to_string(kiteCount));

		DebugOverlay::Instance().FocusStat(gameObject, "lead " + std::to_string(static_cast<int>(profile.leadPrediction * leadBias * 100.f))
			+ "% vel " + std::to_string(static_cast<int>(glm::length(targetVelocity))));

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
