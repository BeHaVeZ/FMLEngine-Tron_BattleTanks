#pragma once
#include <string_view>

namespace FML::Tags
{
	inline constexpr std::string_view Player1 = "Player1";
	inline constexpr std::string_view Player2 = "Player2";
	inline constexpr std::string_view BlueTank = "BlueTank";
	inline constexpr std::string_view PinkTank = "PinkTank";
	inline constexpr std::string_view Recognizer = "Recognizer";
	inline constexpr std::string_view Wall = "Wall";
	inline constexpr std::string_view Bullet = "Bullet";
	inline constexpr std::string_view EnemyBullet = "EnemyBullet";

	inline constexpr int BlueTankScore = 50;
	inline constexpr int PinkTankScore = 100;
	inline constexpr int RecognizerScore = 200;

	constexpr bool IsEnemyTag(const std::string_view tag)
	{
		return tag == BlueTank || tag == PinkTank || tag == Recognizer;
	}

	constexpr int ScoreForTag(const std::string_view tag)
	{
		if (tag == BlueTank)
			return BlueTankScore;
		if (tag == PinkTank)
			return PinkTankScore;
		if (tag == Recognizer)
			return RecognizerScore;
		return 0;
	}

	constexpr bool IsPlayerTag(const std::string_view tag)
	{
		return tag == Player1 || tag == Player2;
	}
}
