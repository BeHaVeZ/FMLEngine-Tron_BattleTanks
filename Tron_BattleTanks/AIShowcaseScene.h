#pragma once
#include "TankLevelScene.h"

namespace FML
{
	class AIShowcaseScene final : public TankLevelScene
	{
	public:
		AIShowcaseScene();

		bool Initialize(SDL_Renderer* renderer) override;
		void InitializeInput() override;

	protected:
		void InitializePlayer() override;
		void InitializeUI() override;
	};
}
