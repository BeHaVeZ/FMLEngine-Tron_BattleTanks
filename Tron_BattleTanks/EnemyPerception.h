#pragma once
#include "glm.hpp"

namespace FML
{
	class GameObject;

	namespace EnemyPerception
	{
		GameObject* SeePlayerAhead(GameObject* agent, float range);

		bool AllyInLineOfFire(GameObject* shooter, const glm::vec2& origin, const glm::vec2& forward, float range);
	}
}
