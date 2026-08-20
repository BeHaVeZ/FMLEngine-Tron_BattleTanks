#include "PauseMenu.h"
#include <algorithm>
#include "ConfigManager.h"
#include "GameStateManager.h"
#include "InputHandler.h"
#include "SceneManager.h"
#include "ServiceLocator.h"
#include "SoundHelper.h"
#include "TextComponent.h"
#include "TransformComponent.h"

namespace FML
{
	namespace
	{
		constexpr SDL_Color menuColor{ 0, 255, 255, 255 };
		const std::string menuFont = "data/fonts/tron-arcade.ttf";
	}

	PauseMenu& PauseMenu::Instance()
	{
		static PauseMenu instance;
		return instance;
	}

	void PauseMenu::Toggle()
	{
		if (isOpen)
			Close();
		else
			Open();
	}

	void PauseMenu::Open()
	{
		if (isOpen) return;

		SDL_Renderer* renderer = SceneManager::Instance().GetRenderer();
		if (!renderer) return;

		isOpen = true;
		selectedIndex = 0;

		GameStateManager::Instance().SetPaused(true);
		InputHandler::Instance().SetCommandsSuspended(true);
		ServiceLocator::GetSoundSystem().SetMusicVolumeScale(musicDuck);

		BuildItems();
		SpawnItemObjects(renderer);
		UpdateHighlight();
	}

	void PauseMenu::Close()
	{
		if (!isOpen) return;

		isOpen = false;
		itemObjects.clear();
		titleObject.reset();
		items.clear();
		selectedIndex = 0;

		ServiceLocator::GetSoundSystem().SetMusicVolumeScale(1.f);
		InputHandler::Instance().SetCommandsSuspended(false);
		GameStateManager::Instance().SetPaused(false);
	}

	void PauseMenu::BuildItems()
	{
		items.clear();

		items.push_back({ "RESUME", []() { Instance().Close(); } });

		items.push_back({ "RESTART LEVEL", []()
			{
				Instance().Close();
				SceneManager::Instance().ReloadScene();
			} });

		items.push_back({ "MAIN MENU", []()
			{
				Instance().Close();
				SceneManager::Instance().QueueSceneChange("MainMenu");
			} });

		items.push_back({ "QUIT", []()
			{
				Instance().Close();
				GameStateManager::Instance().SetRunning(false);
			} });
	}

	void PauseMenu::SpawnItemObjects(SDL_Renderer* renderer)
	{
		itemObjects.clear();
		titleObject.reset();

		const float windowWidth = static_cast<float>(ConfigManager::Instance().GetWindowWidth());
		const float windowHeight = static_cast<float>(ConfigManager::Instance().GetWindowHeight());

		const float blockHeight = static_cast<float>(titleToFirstRow + static_cast<int>(items.size()) * rowSpacing);
		const float blockTop = (windowHeight - blockHeight) * .5f;

		int widest = 0;
		auto spawnText = [&](const std::string& content, int size, float rowY)
		{
			auto object = std::make_unique<GameObject>(content);
			auto text = std::make_unique<TextComponent>(content, menuFont, size, menuColor, renderer);
			widest = (std::max)(widest, text->GetWidth());
			const float centeredX = (windowWidth - static_cast<float>(text->GetWidth())) * .5f;
			object->AddComponent(std::move(text));
			object->GetComponent<TransformComponent>()->SetPosition({ centeredX, rowY });
			return object;
		};

		titleObject = spawnText("PAUSED", titleFontSize, blockTop);

		for (size_t i = 0; i < items.size(); ++i)
		{
			const float rowY = blockTop + static_cast<float>(titleToFirstRow + static_cast<int>(i) * rowSpacing);
			itemObjects.push_back(spawnText(items[i].label, fontSize, rowY));
		}

		const int panelWidth = widest + panelPadding * 2;
		const int panelHeight = static_cast<int>(blockHeight) + panelPadding * 2;
		panelRect = SDL_Rect{
			static_cast<int>((windowWidth - static_cast<float>(panelWidth)) * .5f),
			static_cast<int>(blockTop) - panelPadding,
			panelWidth,
			panelHeight
		};
	}

	void PauseMenu::UpdateHighlight()
	{
		for (size_t i = 0; i < itemObjects.size(); ++i)
		{
			if (auto* text = itemObjects[i]->GetComponent<TextComponent>())
			{
				text->SetAlpha(i == selectedIndex ? selectedAlpha : dimmedAlpha);
			}
		}
	}

	void PauseMenu::MoveSelection(int delta)
	{
		if (!isOpen || items.empty()) return;

		const int count = static_cast<int>(items.size());
		const int shifted = static_cast<int>(selectedIndex) + delta;
		selectedIndex = static_cast<size_t>(((shifted % count) + count) % count);

		SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });
		UpdateHighlight();
	}

	void PauseMenu::Activate()
	{
		if (!isOpen || selectedIndex >= items.size()) return;

		SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });

		const auto activate = items[selectedIndex].onActivate;
		if (activate) activate();
	}

	void PauseMenu::Render(SDL_Renderer* renderer)
	{
		if (!isOpen || !renderer) return;

		SDL_BlendMode previousBlendMode;
		SDL_GetRenderDrawBlendMode(renderer, &previousBlendMode);
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, scrimAlpha);

		const SDL_Rect scrim{
			0,
			0,
			ConfigManager::Instance().GetWindowWidth(),
			ConfigManager::Instance().GetWindowHeight()
		};
		SDL_RenderFillRect(renderer, &scrim);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, panelAlpha);
		SDL_RenderFillRect(renderer, &panelRect);
		SDL_SetRenderDrawBlendMode(renderer, previousBlendMode);

		if (titleObject) titleObject->Render(renderer);
		for (auto& object : itemObjects)
		{
			object->Render(renderer);
		}
	}
}
