#pragma once
#include "Component.h"
#include "TextComponent.h"
#include "SceneManager.h"
#include <string>

namespace FML
{
	class ScoreUIComponent : public Component
	{
	public:
		explicit ScoreUIComponent(int score = 0) : displayedScore(score)
		{
			textComponent = std::make_unique<TextComponent>(std::to_string(displayedScore), "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,255,0,255 }, SceneManager::Instance().GetRenderer());
		}

		ScoreUIComponent(const int* source, SDL_Color color)
			: source(source), displayedScore(source ? *source : 0)
		{
			textComponent = std::make_unique<TextComponent>(std::to_string(displayedScore), "data/fonts/tron-arcade.ttf", 20, color, SceneManager::Instance().GetRenderer());
		}

		void Initialize() override 
		{
			gameObject->AddComponent(std::move(textComponent));
			text = gameObject->GetComponent<TextComponent>();
		}

		void Update(float) override
		{
			if (!source || *source == displayedScore)
				return;

			displayedScore = *source;
			if (text)
			{
				text->SetText(std::to_string(displayedScore), SceneManager::Instance().GetRenderer());
			}
		}

	private:
		std::unique_ptr<TextComponent> textComponent;
		TextComponent* text{ nullptr };
		const int* source{ nullptr };
		int displayedScore;
	};
}
