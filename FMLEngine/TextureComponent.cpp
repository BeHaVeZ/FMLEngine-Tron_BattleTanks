#include "TextureComponent.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Logger.h"
#include "ScreenShake.h"

namespace FML
{

	TextureComponent::TextureComponent(const std::string& filePath, SDL_Renderer* renderer)
	{
		if (!TextureManager::Instance().Load(filePath, filePath, renderer))
		{
			Logger::Log(LogLevel::Error, "Failed to load texture: %s", filePath.c_str());
			return;
		}

		texture = TextureManager::Instance().GetTexture(filePath);

		int width = 0;
		int height = 0;
		if (!texture || SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) != 0)
		{
			Logger::Log(LogLevel::Error, "Failed to query texture %s: %s", filePath.c_str(), SDL_GetError());
			texture = nullptr;
			return;
		}
		defaultWidth = width;
		defaultHeight = height;

		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		destRect = { 0, 0, width, height };
		srcRect = { 0, 0, defaultWidth, defaultHeight };
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

				const glm::vec2 shake = ScreenShake::Instance().GetOffset();

				destRect.x = static_cast<int>(transform->GetWorldPosition().x - center.x + shake.x);
				destRect.y = static_cast<int>(transform->GetWorldPosition().y - center.y + shake.y);

				SDL_SetTextureAlphaMod(texture, alpha);

				SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect,
					transform->GetWorldRotation(), &center, SDL_FLIP_NONE);
			}
		}
	}

	void TextureComponent::SetAlpha(Uint8 newAlpha)
	{
		alpha = newAlpha;
	}

	void TextureComponent::SetSourceRect(const SDL_Rect& rect)
	{
		srcRect = rect;
	}

	SDL_Rect TextureComponent::GetSourceRect() const
	{
		return srcRect;
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

