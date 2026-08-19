#pragma once
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <cmath>

namespace FML
{
	class TurretAimComponent final : public Component
	{
	public:
		void Update(float) override
		{
			auto* transform = gameObject ? gameObject->GetComponent<TransformComponent>() : nullptr;
			if (!transform)
				return;

			const float hullAngle = HullAngle();

			if (!aimInitialized)
			{
				aim = hullAngle + transform->GetLocalRotation();
				aimInitialized = true;
			}

			transform->SetRotation(aim - hullAngle);
			transform->UpdateWorldPosition();
		}

		void Rotate(float degrees)
		{
			aim = Normalize(aim + degrees);
			aimInitialized = true;
		}

		void SetAim(float degrees)
		{
			aim = Normalize(degrees);
			aimInitialized = true;
		}

		float GetAim() const { return aim; }

	private:
		float HullAngle() const
		{
			GameObject* parent = gameObject->GetParent();
			if (!parent)
				return 0.f;

			auto* parentTransform = parent->GetComponent<TransformComponent>();
			return parentTransform ? -parentTransform->GetWorldRotation() : 0.f;
		}

		static float Normalize(float degrees)
		{
			degrees = std::fmod(degrees + 180.f, 360.f);
			if (degrees < 0.f)
				degrees += 360.f;

			return degrees - 180.f;
		}

		float aim{ 0.f };
		bool aimInitialized{ false };
	};
}
