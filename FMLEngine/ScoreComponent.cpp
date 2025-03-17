#include "ScoreComponent.h"
#include "SceneManager.h"
#include <stdexcept>

ScoreComponent::ScoreComponent() {}

void ScoreComponent::Initialize() {
    if (!gameObject) {
        throw std::runtime_error("GameObject is null");
        return;
    }
    if (TextComponent* text = gameObject->GetComponent<TextComponent>()) {
        text->SetText(std::to_string(score), SceneManager::Instance().GetRenderer());
    }
}