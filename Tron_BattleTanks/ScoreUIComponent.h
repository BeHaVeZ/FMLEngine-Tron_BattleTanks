#pragma once
#include "Component.h"
#include "TextComponent.h"
#include "SceneManager.h"
#include "BlueTankKilledEvent.h"

namespace FML
{
	class ScoreUIComponent : public Component, public Observer
	{
	public:
		ScoreUIComponent() : text(nullptr),currentScore(0)
		{
			textComponent = std::make_unique<TextComponent>(std::to_string(currentScore), "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,255,0,255 }, SceneManager::Instance().GetRenderer());
		}

		~ScoreUIComponent()
		{
		}

		void Initialize() override 
		{
			gameObject->AddComponent(std::move(textComponent));
			text = gameObject->GetComponent<TextComponent>();
		}

		void HandleEvent(const Event& event) override 
		{
			if (const BlueTankKilledEvent* enemyKilledEvent = dynamic_cast<const BlueTankKilledEvent*>(&event))
			{
				currentScore += enemyKilledEvent->GetScore();
				if (text) 
				{
					text->SetText(std::to_string(currentScore), SceneManager::Instance().GetRenderer());
					Logger::Log(LogLevel::Info, "HealthUIComponent Health updated to %d", currentScore);
				}
			}
		}

	private:
		std::unique_ptr<TextComponent> textComponent;
		TextComponent* text;
		int currentScore;
	};
}