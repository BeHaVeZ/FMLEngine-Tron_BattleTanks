#include "TextureComponent.h"
#include "TextureComponent.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <iostream>

namespace FML
{

	TextureComponent::TextureComponent(const std::string& filePath, SDL_Renderer* renderer)
		: texture(nullptr) {
		if (!TextureManager::Instance().Load(filePath, filePath, renderer)) {
			printf("Failed to load texture in TextureComponent constructor\n");
		}

		texture = TextureManager::Instance().GetTexture(filePath);

		int width, height;
		SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
		defaultWidth = width;
		defaultHeight = height;

		destRect = { 0, 0, width, height };
	}

	void TextureComponent::Render(SDL_Renderer* renderer) {
		if (texture) {
			auto transform = gameObject->GetComponent<TransformComponent>();
			if (transform) {
				destRect.w = static_cast<int>(transform->IsSizeSet() ? transform->GetWidth() : defaultWidth);
				destRect.h = static_cast<int>(transform->IsSizeSet() ? transform->GetHeight() : defaultHeight);

				SDL_Point center{
					static_cast<int>(transform->GetPivot().x * destRect.w),
					static_cast<int>(transform->GetPivot().y * destRect.h)
				};

				destRect.x = static_cast<int>(transform->GetWorldPosition().x - center.x);
				destRect.y = static_cast<int>(transform->GetWorldPosition().y - center.y);

				SDL_RenderCopyEx(renderer, texture, nullptr, &destRect,
					transform->GetWorldRotation(), &center, SDL_FLIP_NONE);
			}
		}
	}

	glm::vec2 TextureComponent::GetWorldCenter() const
	{
		if (!gameObject) return { 0, 0 };

		auto transform = gameObject->GetComponent<TransformComponent>();
		if (!transform) return { 0, 0 };

		float width = transform->IsSizeSet() ? transform->GetWidth() : defaultWidth;
		float height = transform->IsSizeSet() ? transform->GetHeight() : defaultHeight;

		glm::vec2 position = transform->GetWorldPosition();
		return { position.x + width / 2.0f, position.y + height / 2.0f };
	}

	glm::vec2 TextureComponent::GetForwardVector() const
	{
		if (!gameObject) return { 0, -1 };

		auto transform = gameObject->GetComponent<TransformComponent>();
		if (!transform) return { 0, -1 };

		float rotation = transform->GetWorldRotation();
		float angleRadians = glm::radians(rotation - 90.0f);

		glm::vec2 forward = glm::vec2(std::cos(angleRadians), std::sin(angleRadians));
		return glm::normalize(forward);
	}

}

