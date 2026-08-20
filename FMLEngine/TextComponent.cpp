#include "TextComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"
#include <iostream>

namespace FML
{
	TextComponent::TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, SDL_Renderer* renderer)
		: text(text), color(color), texture(nullptr), font(nullptr), alpha(255) {
		font = TTF_OpenFont(fontPath.c_str(), fontSize);
		if (!font) {
			std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
		}

		SetText(text, renderer);
	}

	TextComponent::~TextComponent() {
		if (texture) {
			SDL_DestroyTexture(texture);
		}
		if (font) {
			TTF_CloseFont(font);
		}
	}

	void TextComponent::SetText(const std::string& newText, SDL_Renderer* renderer) {
		text = newText;
		if (texture) {
			SDL_DestroyTexture(texture);
		}

		SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
		if (!textSurface) {
			std::cerr << "Unable to render text surface: " << TTF_GetError() << std::endl;
			return;
		}

		texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_FreeSurface(textSurface);

		if (texture) {
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, alpha);
		}
	}

	void TextComponent::SetColor(SDL_Color newColor, SDL_Renderer* renderer)
	{
		if (newColor.r == color.r &&
			newColor.g == color.g &&
			newColor.b == color.b &&
			newColor.a == color.a)
		{
			return; 
		}

		color = newColor;
		SetText(text, renderer);
	}

	void TextComponent::SetAlpha(Uint8 newAlpha)
	{
		alpha = newAlpha;
		if (texture) {
			SDL_SetTextureAlphaMod(texture, alpha);
		}
	}

	int TextComponent::GetWidth() const
	{
		int width = 0;
		if (texture) {
			SDL_QueryTexture(texture, nullptr, nullptr, &width, nullptr);
		}
		return width;
	}

	int TextComponent::GetHeight() const
	{
		int height = 0;
		if (texture) {
			SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &height);
		}
		return height;
	}

	void TextComponent::Render(SDL_Renderer* renderer) {
		if (texture and gameObject) {
			TransformComponent* transform = gameObject->GetComponent<TransformComponent>();
			if (transform) {
				SDL_Rect renderQuad = { (int)transform->GetLocalPosition().x, (int)transform->GetLocalPosition().y, 0, 0 };
				SDL_QueryTexture(texture, NULL, NULL, &renderQuad.w, &renderQuad.h);

				SDL_Point center = { renderQuad.w / 2, renderQuad.h / 2 };

				SDL_RenderCopyEx(renderer, texture, NULL, &renderQuad,
					transform->GetLocalRotation(), &center, SDL_FLIP_NONE);
			}
		}
	}

}