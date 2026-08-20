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
		float fireTolerance;
		float threatHorizon;
		float dodgeCommitment;
		float decisionInterval;
		float preferredRange;
		float adaptationRate;
	};

	inline constexpr AIDifficultyProfile EasyProfile
	{
		.reactionDelay = .55f,
		.turretTurnRate = 70.f,
		.aimError = 14.f,
		.leadPrediction = .15f,
		.fireTolerance = 12.f,
		.threatHorizon = .35f,
		.dodgeCommitment = .45f,
		.decisionInterval = 3.5f,
		.preferredRange = 260.f,
		.adaptationRate = .25f,
	};

	inline constexpr AIDifficultyProfile NormalProfile
	{
		.reactionDelay = .25f,
		.turretTurnRate = 110.f,
		.aimError = 6.f,
		.leadPrediction = .6f,
		.fireTolerance = 7.f,
		.threatHorizon = .8f,
		.dodgeCommitment = .75f,
		.decisionInterval = 2.f,
		.preferredRange = 220.f,
		.adaptationRate = .6f,
	};

	inline constexpr AIDifficultyProfile HardProfile
	{
		.reactionDelay = .08f,
		.turretTurnRate = 150.f,
		.aimError = 1.5f,
		.leadPrediction = 1.f,
		.fireTolerance = 4.f,
		.threatHorizon = 1.5f,
		.dodgeCommitment = 1.f,
		.decisionInterval = 1.f,
		.preferredRange = 190.f,
		.adaptationRate = 1.f,
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
