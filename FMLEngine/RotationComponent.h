#pragma once
#include "Component.h"
#include "TransformComponent.h"
#include <cmath>

namespace FML
{

	class RotationComponent : public Component {
	public:
		RotationComponent(float radius, float speed, float centerX = 0.0f, float centerY = 0.0f);

		void Update(float deltaTime) override;
		void Initialize() override;

	private:
		float radius;
		float speed;
		float angle;
		float centerX;
		float centerY;
	};
}