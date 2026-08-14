#pragma once
#include "TankLevelScene.h"

namespace FML
{

	class SoloLevel2 final : public TankLevelScene {
	public:
		SoloLevel2() : TankLevelScene("Solo2", {
			"data/levels/level01.png", "data/levels/level01C.txt", "SoloTheme_2.wav",
			{ 514.f, 428.f }, 4, 1, 2 }) {}
	};

}

