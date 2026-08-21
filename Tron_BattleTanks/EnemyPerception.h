#pragma once
#include "glm.hpp"
#include <string_view>

namespace FML
{
	class GameObject;

	namespace EnemyPerception
	{
		using TagPredicate = bool(*)(std::string_view);

		GameObject* SeePlayerAhead(GameObject* agent, float range);

		bool BlockerInLineOfFire(GameObject* shooter, const glm::vec2& origin, const glm::vec2& forward, float range, TagPredicate isBlocker);

		bool AllyInLineOfFire(GameObject* shooter, const glm::vec2& origin, const glm::vec2& forward, float range);
	}
}
