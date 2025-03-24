#pragma once
#include <SDL_ttf.h>
#include <string>
#include "Component.h"
#include "GameObject.h" 

namespace FML
{

	class TextComponent : public Component {
	public:
		TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, SDL_Renderer* renderer);
		~TextComponent();

		void SetText(const std::string& newText, SDL_Renderer* renderer);
		std::string GetText() const { return text; }
		void Render(SDL_Renderer* renderer) override;

	private:
		std::string text;
		TTF_Font* font;
		SDL_Texture* texture;
		SDL_Color color;
	};

}