#pragma once
#include "GameData.h"

namespace FML
{
	struct AIDifficultyProfile
	{
		float reactionDelay;
		float turretTurnRate;
		float aimError;
		float leadPrediction;
		float leadError;
		float fireTolerance;
		float threatHorizon;
		float dodgeCommitment;
		float decisionInterval;
		float preferredRange;
		float engageRange;
		float fireDelay;
		float strafeAmount;
		float adaptationRate;
		int bankBounces;
		float bankSearchStep;
		float bankInterval;
	};

	inline constexpr AIDifficultyProfile EasyProfile
	{
		.reactionDelay = .55f,
		.turretTurnRate = 70.f,
		.aimError = 22.f,
		.leadPrediction = .4f,
		.leadError = .6f,
		.fireTolerance = 10.f,
		.threatHorizon = 1.4f,
		.dodgeCommitment = 1.f,
		.decisionInterval = 3.5f,
		.preferredRange = 280.f,
		.engageRange = 200.f,
		.fireDelay = .6f,
		.strafeAmount = 0.f,
		.adaptationRate = .25f,
		.bankBounces = 1,
		.bankSearchStep = 9.f,
		.bankInterval = .9f,
	};

	inline constexpr AIDifficultyProfile NormalProfile
	{
		.reactionDelay = .25f,
		.turretTurnRate = 110.f,
		.aimError = 6.f,
		.leadPrediction = .85f,
		.leadError = .25f,
		.fireTolerance = 7.f,
		.threatHorizon = 1.5f,
		.dodgeCommitment = 1.f,
		.decisionInterval = 2.f,
		.preferredRange = 200.f,
		.engageRange = 140.f,
		.fireDelay = .15f,
		.strafeAmount = .5f,
		.adaptationRate = .6f,
		.bankBounces = 2,
		.bankSearchStep = 5.f,
		.bankInterval = .5f,
	};

	inline constexpr AIDifficultyProfile HardProfile
	{
		.reactionDelay = .08f,
		.turretTurnRate = 150.f,
		.aimError = 2.f,
		.leadPrediction = 1.f,
		.leadError = .05f,
		.fireTolerance = 4.f,
		.threatHorizon = 1.6f,
		.dodgeCommitment = 1.f,
		.decisionInterval = .6f,
		.preferredRange = 170.f,
		.engageRange = 90.f,
		.fireDelay = 0.f,
		.strafeAmount = 1.f,
		.adaptationRate = 1.f,
		.bankBounces = 3,
		.bankSearchStep = 2.5f,
		.bankInterval = .25f,
	};

	constexpr const AIDifficultyProfile& ProfileFor(GameData::AIDifficulty difficulty)
	{
		switch (difficulty)
		{
		case GameData::AIDifficulty::Easy:
			return EasyProfile;
		case GameData::AIDifficulty::Hard:
			return HardProfile;
		default:
			return NormalProfile;
		}
	}

	constexpr const char* DifficultyName(GameData::AIDifficulty difficulty)
	{
		switch (difficulty)
		{
		case GameData::AIDifficulty::Easy:
			return "EASY";
		case GameData::AIDifficulty::Hard:
			return "HARD";
		default:
			return "NORMAL";
		}
	}
}
