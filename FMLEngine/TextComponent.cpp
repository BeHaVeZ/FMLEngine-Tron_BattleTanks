#include "TextComponent.h"
#include <iostream>

TextComponent::TextComponent(const std::string& text, const std::string& fontPath, int fontSize, SDL_Color color, SDL_Renderer* renderer)
    : text(text), color(color), texture(nullptr), font(nullptr) {
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
    textRect = { 0, 0, textSurface->w, textSurface->h };
    SDL_FreeSurface(textSurface);
}

void TextComponent::Render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
    }
}

void TextComponent::SetPosition(int x, int y) {
    textRect.x = x;
    textRect.y = y;
}
