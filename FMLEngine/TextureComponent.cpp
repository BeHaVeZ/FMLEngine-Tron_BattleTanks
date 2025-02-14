#include "TextureComponent.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "TransformComponent.h"

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
            destRect.x = static_cast<int>(transform->GetWorldX());
            destRect.y = static_cast<int>(transform->GetWorldY());

            destRect.w = static_cast<int>(transform->IsSizeSet() ? transform->GetWidth() : defaultWidth);
            destRect.h = static_cast<int>(transform->IsSizeSet() ? transform->GetHeight() : defaultHeight);

            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        }
    }
}

