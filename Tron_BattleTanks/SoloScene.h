#pragma once
#include "TankLevelScene.h"

namespace FML
{

	class SoloScene final : public TankLevelScene {
	public:
		SoloScene() : TankLevelScene("Solo", {
			"data/levels/level00.png", "data/levels/level00C.txt", "SoloTheme_1.wav" }) {}
	};

}

