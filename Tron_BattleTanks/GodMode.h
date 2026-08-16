#pragma once

namespace FML
{
	class GameObject;

	namespace GodMode
	{
		bool IsEnabled();
		void Toggle();

		void Apply(GameObject* player);

		void ApplyToCurrentScene();
	}
}
