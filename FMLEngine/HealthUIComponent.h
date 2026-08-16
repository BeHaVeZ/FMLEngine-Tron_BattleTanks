#pragma once
#include "TextComponent.h"
#include "Component.h"
#include "HealthComponent.h"
#include "Observer.h"
#include "SceneManager.h"
#include "Logger.h"
#include "DamageEvent.h"

namespace FML {
    class HealthUIComponent : public Component, public Observer {
    public:
        HealthUIComponent(const HealthComponent* source, SDL_Color color = {0,0,255,255}) : healthSource(source), text(nullptr), color(color) {
            textComponent = std::make_unique<TextComponent>(std::to_string(CurrentHealth()), "data/fonts/tron-arcade.ttf", 20, SDL_Color{ color }, SceneManager::Instance().GetRenderer());
        }

        void Initialize() override {
            gameObject->AddComponent(std::move(textComponent));
            text = gameObject->GetComponent<TextComponent>();
        }

        void HandleEvent(const Event& event) override {
            if (dynamic_cast<const DamageEvent*>(&event)) {
                if (text) {
                    text->SetText(std::to_string(CurrentHealth()), SceneManager::Instance().GetRenderer());
                    Logger::Log(LogLevel::Info, "HealthUIComponent Health updated to %d", CurrentHealth());
                }
            }
        }

    private:
        int CurrentHealth() const { return healthSource ? healthSource->GetCurrentHealth() : 0; }

        std::unique_ptr<TextComponent> textComponent;
        TextComponent* text;
        const HealthComponent* healthSource;
        SDL_Color color;
    };
}
