#pragma once

namespace FML
{
	class GameObject;

	namespace EnemyPerception
	{
		GameObject* SeePlayerAhead(GameObject* agent, float range);
	}
}
