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

	constexpr bool IsEnemyTag(const std::string_view tag)
	{
		return tag == BlueTank || tag == PinkTank || tag == Recognizer;
	}

	constexpr bool IsPlayerTag(const std::string_view tag)
	{
		return tag == Player1 || tag == Player2;
	}
}
