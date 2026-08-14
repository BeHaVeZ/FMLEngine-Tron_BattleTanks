#pragma once
#include <SDL.h>
#include <string>
#include "Component.h"
#include <glm.hpp>

namespace FML
{

	class TextureComponent : public Component
	{
	public:
		TextureComponent(const std::string& filePath, SDL_Renderer* renderer);
		void Render(SDL_Renderer* renderer) override;

		int GetDefaultWidth() const { return defaultWidth; }
		int GetDefaultHeight() const { return defaultHeight; }


		glm::vec2 GetWorldCenter() const;
		glm::vec2 GetForwardVector() const;


		void SetSourceRect(const SDL_Rect& rect);
		SDL_Rect GetSourceRect() const;

	private:
		SDL_Texture* texture{ nullptr };
		SDL_Rect destRect{};
		SDL_Rect srcRect{};

		int defaultWidth{ 0 };
		int defaultHeight{ 0 };
	};


}