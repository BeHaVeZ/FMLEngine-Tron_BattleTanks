#pragma once
#include "TextComponent.h"
#include "Component.h"
#include "Observer.h"
#include "SceneManager.h"
#include "iostream"

class HealthUIComponent : public Component, public Observer
{
public:
	HealthUIComponent(int health) : currentHealth(health), text(nullptr)
	{
		textComponent = std::make_unique<TextComponent>("P1 Health " + std::to_string(currentHealth), "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255, 255, 255, 255 }, SceneManager::Instance().GetRenderer());
	}

	void Initialize() override
	{
		gameObject->AddComponent(std::move(textComponent));
	}

	void HandleEvent(const DamageEvent& event) override
	{ 
		currentHealth -= event.GetDamage();
		text = gameObject->GetComponent<TextComponent>();
		text->SetText("P1 Health " + std::to_string(currentHealth), SceneManager::Instance().GetRenderer());
		std::cout << "HealthUIComponent: Health updated to " << currentHealth << std::endl;
	}
private:
	std::unique_ptr<TextComponent> textComponent;
	TextComponent* text;
	int currentHealth;
};

