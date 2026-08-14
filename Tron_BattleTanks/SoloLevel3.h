#pragma once
#include "TankLevelScene.h"

namespace FML
{

	class SoloLevel3 final : public TankLevelScene {
	public:
		SoloLevel3() : TankLevelScene("Solo3", {
			"data/levels/level02.png", "data/levels/level02C.txt", "SoloTheme_3.wav",
			{ 514.f, 428.f }, 2, 2, 2 }) {}
	};
}
