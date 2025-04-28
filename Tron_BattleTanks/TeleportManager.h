#pragma once
#include <vector>
#include "glm.hpp"

namespace FML
{
	class TeleportManager
	{
	public:
		static TeleportManager& Instance()
		{
			static TeleportManager instance;
			return instance;
		}

		void InitializeCustomTeleportPositions(const std::vector<glm::vec2>& points);
		glm::vec2 GetRandomTeleportPosition() const;

	private:
		std::vector<glm::vec2> teleportPositions = { {174,205},{378,206},{175,643},{343,649},{648,646},{856,643},{854,356},{652,353},{653,282},{857,203},{955,134} };
	};
}
