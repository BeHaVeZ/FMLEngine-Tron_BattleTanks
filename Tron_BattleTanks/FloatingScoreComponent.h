#pragma once
#include <algorithm>
#include "Component.h"
#include "GameObject.h"
#include "TextComponent.h"
#include "TransformComponent.h"

namespace FML
{
	class FloatingScoreComponent final : public Component
	{
	public:
		explicit FloatingScoreComponent(glm::vec2 origin) : origin(origin)
		{
		}

		void Initialize() override
		{
			text = gameObject->GetComponent<TextComponent>();
			transform = gameObject->GetComponent<TransformComponent>();
		}

		void Update(float deltaTime) override
		{
			elapsed += deltaTime;

			const float t = std::min(elapsed / lifeTime, 1.f);

			const float rise = 1.f - (1.f - t) * (1.f - t);
			if (transform)
			{
				transform->SetPosition({ origin.x, origin.y - riseDistance * rise });
			}

			if (text and t > fadeStart)
			{
				const float fade = (t - fadeStart) / (1.f - fadeStart);
				text->SetAlpha(static_cast<Uint8>((1.f - fade) * 255.f));
			}

			if (t >= 1.f)
			{
				gameObject->Destroy();
			}
		}

	private:
		glm::vec2 origin;
		TextComponent* text{ nullptr };
		TransformComponent* transform{ nullptr };
		float elapsed{ 0.f };

		static constexpr float lifeTime{ 0.9f };
		static constexpr float riseDistance{ 40.f };
		static constexpr float fadeStart{ 0.5f };
	};
}
