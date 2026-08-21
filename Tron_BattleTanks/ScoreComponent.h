#pragma once
#include "Component.h"
#include "GameData.h"
#include "GameObject.h"
#include "GameTags.h"
#include <string_view>

namespace FML
{
	class ScoreComponent : public Component
	{
	public:
		void Initialize() override
		{
			if (gameObject)
			{
				stats = GameData::StatsForPlayer(Tags::PlayerNumberForTag(gameObject->GetTag()));
			}
		}

		void AddKill(const std::string_view enemyTag, int killScore)
		{
			if (!stats)
				return;

			if (enemyTag == Tags::BlueTank)
				++stats->blueTankKills;
			else if (enemyTag == Tags::PinkTank)
				++stats->pinkTankKills;
			else if (enemyTag == Tags::Recognizer)
				++stats->recognizerKills;

			stats->score += killScore;
		}

		int GetCurrentScore() const { return stats ? stats->score : 0; }

	private:
		GameData::PlayerStats* stats{ nullptr };
	};

}
