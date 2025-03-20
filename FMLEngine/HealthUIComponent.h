#pragma once
#include "TextComponent.h"
#include "Component.h"
#include "Observer.h"
#include "SceneManager.h"
#include "iostream"

class HealthUIComponent : public Component, public Observer
{
public:
	HealthUIComponent(int health) : currentHealth(health)
	{
		textComponent = std::make_unique<TextComponent>("Health " + std::to_string(currentHealth), "data/fonts/tron-arcade.ttf", 10, SDL_Color{ 255, 255, 255, 255 }, SceneManager::Instance().GetRenderer());
	}

	void Initialize() override
	{
		gameObject->AddComponent(std::move(textComponent));
	}

	void OnNotify(const GameObject&, Event event) override
	{
		if (event.type == EventType::DamageTaken)
		{
			auto damageEvent = std::get<DamageEvent>(event.data);
			currentHealth -= damageEvent.damageAmount;
			textComponent->SetText("Health: " + std::to_string(currentHealth), SceneManager::Instance().GetRenderer());
			std::cout << "HealthUIComponent: Health updated to " << currentHealth << std::endl;
		}
	}
/// <summary>
/// TEXTCOMPONENT IS MOVED SO IT IS EMPTY WHEN IT TRIES TO Change text 
/// </summary>

private:
	std::unique_ptr<TextComponent> textComponent;
	int currentHealth;
};

