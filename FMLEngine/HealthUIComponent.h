#pragma once
#include "TextComponent.h"
#include "Component.h"
#include "Observer.h"
#include "SceneManager.h"
#include "Logger.h"
#include "DamageEvent.h" 

namespace FML {
    class HealthUIComponent : public Component, public Observer {
    public:
        HealthUIComponent(int health) : currentHealth(health), text(nullptr) {
            textComponent = std::make_unique<TextComponent>("P1 Health " + std::to_string(currentHealth), "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 0, 0, 255, 255 }, SceneManager::Instance().GetRenderer());
        }

        void Initialize() override {
            gameObject->AddComponent(std::move(textComponent));
            text = gameObject->GetComponent<TextComponent>();
        }

        void HandleEvent(const Event& event) override {
            if (const DamageEvent* damageEvent = dynamic_cast<const DamageEvent*>(&event)) {
                currentHealth -= damageEvent->GetDamage();
                if (text) {
                    text->SetText("P1 Health " + std::to_string(currentHealth), SceneManager::Instance().GetRenderer());
                    Logger::Log(LogLevel::Info, "HealthUIComponent Health updated to %d", currentHealth);
                }
            }
        }

    private:
        std::unique_ptr<TextComponent> textComponent;
        TextComponent* text;
        int currentHealth;
    };
}
