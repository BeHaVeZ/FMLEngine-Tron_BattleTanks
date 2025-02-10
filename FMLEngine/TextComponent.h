#pragma once
#include <SDL_ttf.h>
#include <string>
#include "Component.h"

class TextComponent : public Component {
public:
    TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, SDL_Renderer* renderer);
    ~TextComponent();

    void SetText(const std::string& newText, SDL_Renderer* renderer);
    std::string GetText() const { return text; }
    void Render(SDL_Renderer* renderer) override;
    void SetPosition(int x, int y);
    SDL_Rect GetTextRect() const { return textRect; }

private:
    std::string text;
    TTF_Font* font;
    SDL_Texture* texture;
    SDL_Color color;
    SDL_Rect textRect;
};
