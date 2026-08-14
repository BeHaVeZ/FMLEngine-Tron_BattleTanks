#pragma once
#include "TankLevelScene.h"

namespace FML
{

	class Level404 final : public TankLevelScene {
	public:
		Level404() : TankLevelScene("Level404", {
			"data/levels/level404.png", "data/levels/level02C.txt", "SoloTheme_404.mp3",
			{ 514.f, 428.f }, 0, 0, 10, .5f }) {}
	};
}