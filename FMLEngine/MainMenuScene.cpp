#include "MainMenuScene.h"
#include "TextComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "GameStateManager.h"
#include "SceneManager.h"
#include <algorithm>
#include <memory>
#include "ConfigManager.h"
#include "ServiceLocator.h"
#include "../Tron_BattleTanks/InputBindingHelper.h"
#include "../Tron_BattleTanks/GameData.h"
#include "SoundHelper.h"

namespace
{
	size_t CycleIndex(size_t current, size_t count, int direction)
	{
		if (count == 0) return 0;

		const int shifted = static_cast<int>(current) + direction;
		const int wrapped = ((shifted % static_cast<int>(count)) + static_cast<int>(count)) % static_cast<int>(count);
		return static_cast<size_t>(wrapped);
	}
}

namespace FML
{

	bool MainMenuScene::Initialize(SDL_Renderer* renderer)
	{
		GameData::CurrentGameMode = GameData::GameMode::None;
		GameData::CurrentScore = 0;
		this->storedRenderer = renderer;

		InitializeBackground(renderer);
		InitializeInput();
		InitializeSounds();

		GameData::ResetValues();

		hasPendingPage = false;
		BuildPage(MenuPage::Root);
		return true;
	}


	void MainMenuScene::HandleInput(SDL_Event& event)
	{
		HandleMouseInput(event);
		InputHandler::Instance().HandleInput(event);
	}

	void MainMenuScene::InitializeInput()
	{
		InputBindingHelper::BindGlobalCommands();

		int controllerID = 0;

		auto& input = InputHandler::Instance();

		input.BindFunction(SDLK_w, [this]() {MoveSelection(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_UP, [this]() {MoveSelection(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_s, [this]() {MoveSelection(1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_DOWN, [this]() {MoveSelection(1); }, InputHandler::KeyAction::KeyUp);

		input.BindFunction(SDLK_a, [this]() {Adjust(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_LEFT, [this]() {Adjust(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_d, [this]() {Adjust(1); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_RIGHT, [this]() {Adjust(1); }, InputHandler::KeyAction::KeyUp);

		input.BindFunction(SDLK_RETURN, [this]() {Activate(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_SPACE, [this]() {Activate(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_e, [this]() {Activate(); }, InputHandler::KeyAction::KeyUp);

		input.BindFunction(SDLK_ESCAPE, [this]() {GoBack(); }, InputHandler::KeyAction::KeyUp);
		input.BindFunction(SDLK_BACKSPACE, [this]() {GoBack(); }, InputHandler::KeyAction::KeyUp);

		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_UP, [this]() {MoveSelection(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_DOWN, [this]() {MoveSelection(1); }, InputHandler::KeyAction::KeyUp);
		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_LEFT, [this]() {Adjust(-1); }, InputHandler::KeyAction::KeyUp);
		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_DPAD_RIGHT, [this]() {Adjust(1); }, InputHandler::KeyAction::KeyUp);
		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_A, [this]() {Activate(); }, InputHandler::KeyAction::KeyUp);
		input.BindGamepadFunction(controllerID, XINPUT_GAMEPAD_B, [this]() {GoBack(); }, InputHandler::KeyAction::KeyUp);
	}


	void MainMenuScene::Update(float deltaTime)
	{
		if (hasPendingPage)
		{
			hasPendingPage = false;
			BuildPage(pendingPage);
		}

		Scene::Update(deltaTime);
	}

	void MainMenuScene::Render(SDL_Renderer* renderer)
	{
		Scene::Render(renderer);
	}

	void MainMenuScene::InitializeBackground(SDL_Renderer* renderer)
	{
		auto background = std::make_unique<GameObject>();
		auto backgroundTexture = std::make_unique<TextureComponent>("data/artassets/tron_bg.png", renderer);
		background->AddComponent(std::move(backgroundTexture));

		auto backgroundTransform = background->GetComponent<TransformComponent>();
		if (backgroundTransform) {
			backgroundTransform->SetPosition({ ConfigManager::Instance().GetWindowWidth() / 2, ConfigManager::Instance().GetWindowHeight() / 2 });
			backgroundTransform->SetSize(
				static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
				static_cast<float>(ConfigManager::Instance().GetWindowHeight())
			);
		}
		gameObjects.push_back(std::move(background));
	}

	void MainMenuScene::QueuePage(MenuPage page)
	{
		pendingPage = page;
		hasPendingPage = true;
	}

	void MainMenuScene::BuildPage(MenuPage page)
	{
		ClearItemObjects();

		currentPage = page;
		selectedIndex = 0;
		items.clear();

		switch (page)
		{
		case MenuPage::Root:     BuildRootItems();     break;
		case MenuPage::Play:     BuildPlayItems();     break;
		case MenuPage::Settings: BuildSettingsItems(); break;
		}

		SpawnItemObjects();
		UpdateHighlight();
	}

	void MainMenuScene::BuildRootItems()
	{
		items.push_back({ "PLAY", { 0, 255, 255, 255 }, [this]() {QueuePage(MenuPage::Play); }, nullptr, nullptr });
		items.push_back({ "SETTINGS", { 0, 255, 255, 255 }, [this]() {QueuePage(MenuPage::Settings); }, nullptr, nullptr });
		items.push_back({ "QUIT", { 0, 255, 255, 255 }, []() {GameStateManager::Instance().SetRunning(false); }, nullptr, nullptr });
	}

	void MainMenuScene::BuildPlayItems()
	{
		items.push_back({ "SOLO", { 0, 255, 255, 255 }, []()
			{
				GameData::CurrentGameMode = GameData::GameMode::Solo;
				SceneManager::Instance().QueueSceneChange("Solo");
			}, nullptr, nullptr });

		items.push_back({ "COOP", { 0, 255, 0, 255 }, []()
			{
				GameData::CurrentGameMode = GameData::GameMode::Coop;
				SceneManager::Instance().QueueSceneChange("Coop");
			}, nullptr, nullptr });

		items.push_back({ "VERSUS", { 255, 0, 0, 255 }, []()
			{
				GameData::CurrentGameMode = GameData::GameMode::Versus;
				SceneManager::Instance().QueueSceneChange("Versus");
			}, nullptr, nullptr });

		items.push_back({ "BACK", { 0, 255, 255, 255 }, [this]() {GoBack(); }, nullptr, nullptr });
	}

	void MainMenuScene::BuildSettingsItems()
	{
		const SDL_Color base{ 0, 255, 255, 255 };

		auto addSettingsRow = [this, base](std::string label, std::function<std::string()> valueText, std::function<void(int)> adjust)
		{
			MenuItem item{ std::move(label), base, [adjust]() {adjust(1); }, std::move(valueText), adjust };
			items.push_back(std::move(item));
		};

		addSettingsRow("LIMIT FPS",
			[]()
			{
				const int limit = ConfigManager::Instance().GetFpsLimit();
				return limit <= 0 ? std::string("UNLIMITED") : std::to_string(limit);
			},
			[](int direction)
			{
				auto& config = ConfigManager::Instance();
				const auto& options = ConfigManager::GetFpsLimitOptions();

				const auto found = std::find(options.begin(), options.end(), config.GetFpsLimit());
				const size_t current = found == options.end() ? 0 : static_cast<size_t>(std::distance(options.begin(), found));
				config.SetFpsLimit(options[CycleIndex(current, options.size(), direction)]);
			});

		addSettingsRow("FULLSCREEN",
			[]() {return ConfigManager::Instance().IsFullscreen() ? std::string("ON") : std::string("OFF"); },
			[](int)
			{
				auto& config = ConfigManager::Instance();
				config.SetFullscreen(!config.IsFullscreen());
				config.ApplyDisplaySettings();
			});

		addSettingsRow("RESOLUTION",
			[]()
			{
				auto& config = ConfigManager::Instance();
				return std::to_string(config.GetDisplayWidth()) + "X" + std::to_string(config.GetDisplayHeight());
			},
			[](int direction)
			{
				auto& config = ConfigManager::Instance();
				const auto& options = ConfigManager::GetResolutionOptions();

				const std::pair<int, int> resolution{ config.GetDisplayWidth(), config.GetDisplayHeight() };
				const auto found = std::find(options.begin(), options.end(), resolution);
				const size_t current = found == options.end() ? 0 : static_cast<size_t>(std::distance(options.begin(), found));

				const auto& picked = options[CycleIndex(current, options.size(), direction)];
				config.SetDisplayResolution(picked.first, picked.second);
				config.ApplyDisplaySettings();
			});

		addSettingsRow("SCALING",
			[]() {return ConfigManager::Instance().GetScalingMode() == ScalingMode::Fit ? std::string("FIT") : std::string("STRETCH"); },
			[](int)
			{
				auto& config = ConfigManager::Instance();
				config.SetScalingMode(config.GetScalingMode() == ScalingMode::Fit ? ScalingMode::Stretch : ScalingMode::Fit);
				config.ApplyScaling();
			});

		addSettingsRow("VOLUME",
			[]()
			{
				const int percentage = static_cast<int>(ConfigManager::Instance().GetMasterVolume() * 100.f + 0.5f);
				return std::to_string(percentage);
			},
			[](int direction)
			{
				auto& config = ConfigManager::Instance();
				config.SetMasterVolume(config.GetMasterVolume() + direction * 0.1f);
				config.ApplyVolume();
			});

		items.push_back({ "BACK", base, [this]() {GoBack(); }, nullptr, nullptr });
	}

	void MainMenuScene::SpawnItemObjects()
	{
		auto spawnText = [this](const std::string& content, const SDL_Color& color, int x, int y)
		{
			auto object = std::make_unique<GameObject>(content);
			auto text = std::make_unique<TextComponent>(content, "data/fonts/tron-arcade.ttf", fontSize, color, storedRenderer);
			object->GetComponent<TransformComponent>()->SetPosition({ x, y });
			object->AddComponent(std::move(text));

			GameObject* raw = object.get();
			gameObjects.push_back(std::move(object));
			return raw;
		};

		for (size_t i = 0; i < items.size(); ++i)
		{
			const MenuItem& item = items[i];
			const int rowY = firstRowY + static_cast<int>(i) * rowSpacing;

			itemObjects.push_back(spawnText(item.label, item.color, columnX, rowY));

			valueObjects.push_back(item.valueText ? spawnText(item.valueText(), item.color, valueColumnX, rowY) : nullptr);
		}
	}

	void MainMenuScene::ClearItemObjects()
	{
		gameObjects.erase(
			std::remove_if(gameObjects.begin(), gameObjects.end(),
				[this](const std::unique_ptr<GameObject>& gameObject)
				{
					return std::find(itemObjects.begin(), itemObjects.end(), gameObject.get()) != itemObjects.end()
						|| std::find(valueObjects.begin(), valueObjects.end(), gameObject.get()) != valueObjects.end();
				}),
			gameObjects.end());

		itemObjects.clear();
		valueObjects.clear();
	}

	void MainMenuScene::RefreshItemValue(size_t index)
	{
		if (index >= valueObjects.size() || !valueObjects[index]) return;
		if (!items[index].valueText) return;

		if (auto* text = valueObjects[index]->GetComponent<TextComponent>())
		{
			text->SetText(items[index].valueText(), storedRenderer);
		}
	}

	void MainMenuScene::UpdateHighlight()
	{
		for (size_t i = 0; i < itemObjects.size(); ++i)
		{
			SDL_Color color = items[i].color;
			color.a = (i == selectedIndex) ? selectedAlpha : dimmedAlpha;

			if (auto* text = itemObjects[i]->GetComponent<TextComponent>())
			{
				text->SetColor(color, storedRenderer);
			}

			if (valueObjects[i])
			{
				if (auto* valueText = valueObjects[i]->GetComponent<TextComponent>())
				{
					valueText->SetColor(color, storedRenderer);
				}
			}
		}
	}

	void MainMenuScene::MoveSelection(int delta)
	{
		if (items.empty()) return;

		const size_t next = CycleIndex(selectedIndex, items.size(), delta);
		if (next == selectedIndex) return;

		selectedIndex = next;
		SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });
		UpdateHighlight();
	}

	void MainMenuScene::Adjust(int direction)
	{
		if (selectedIndex >= items.size()) return;
		if (!items[selectedIndex].onAdjust) return;

		items[selectedIndex].onAdjust(direction);
		RefreshItemValue(selectedIndex);
		UpdateHighlight();
		SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });
	}

	void MainMenuScene::Activate()
	{
		if (selectedIndex >= items.size()) return;

		SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });

		const auto activate = items[selectedIndex].onActivate;
		if (activate) activate();

		if (!hasPendingPage) RefreshItemValue(selectedIndex);
	}

	void MainMenuScene::GoBack()
	{
		if (currentPage == MenuPage::Root) return;

		if (currentPage == MenuPage::Settings)
		{
			ConfigManager::Instance().Save();
		}

		QueuePage(MenuPage::Root);
	}

	void MainMenuScene::HandleMouseInput(const SDL_Event& event)
	{
		if (event.type != SDL_MOUSEMOTION && event.type != SDL_MOUSEBUTTONDOWN) return;

		const int windowX = (event.type == SDL_MOUSEMOTION) ? event.motion.x : event.button.x;
		const int windowY = (event.type == SDL_MOUSEMOTION) ? event.motion.y : event.button.y;

		float x = 0.f;
		float y = 0.f;
		ConfigManager::Instance().WindowToLogical(windowX, windowY, x, y);

		size_t hoveredIndex = 0;
		if (!FindItemAt(x, y, hoveredIndex)) return;

		if (event.type == SDL_MOUSEMOTION)
		{
			if (hoveredIndex == selectedIndex) return;

			selectedIndex = hoveredIndex;
			SoundHelper::PlayRandomSound({ SoundId::UiBlunk1, SoundId::UiBlunk2, SoundId::UiBlunk3 });
			UpdateHighlight();
			return;
		}

		if (event.button.button != SDL_BUTTON_LEFT) return;

		selectedIndex = hoveredIndex;
		UpdateHighlight();
		Activate();
	}

	bool MainMenuScene::FindItemAt(float x, float y, size_t& outIndex) const
	{
		for (size_t i = 0; i < itemObjects.size(); ++i)
		{
			auto* transform = itemObjects[i]->GetComponent<TransformComponent>();
			auto* text = itemObjects[i]->GetComponent<TextComponent>();
			if (!transform || !text) continue;

			const glm::vec2 position = transform->GetLocalPosition();
			float right = position.x + static_cast<float>(text->GetWidth());
			const float height = static_cast<float>(text->GetHeight());

			if (valueObjects[i])
			{
				if (auto* valueText = valueObjects[i]->GetComponent<TextComponent>())
				{
					right = (std::max)(right, static_cast<float>(valueColumnX + valueText->GetWidth()));
				}
			}

			if (x >= position.x && x <= right && y >= position.y && y <= position.y + height)
			{
				outIndex = i;
				return true;
			}
		}
		return false;
	}


	void MainMenuScene::InitializeSounds()
	{
		ServiceLocator::GetSoundSystem().AddSound("blunk_1.wav", SoundId::UiBlunk1, false);
		ServiceLocator::GetSoundSystem().AddSound("blunk_2.wav", SoundId::UiBlunk2, false);
		ServiceLocator::GetSoundSystem().AddSound("blunk_3.wav", SoundId::UiBlunk3, false);
		ConfigManager::Instance().ApplyVolume();
		ServiceLocator::GetSoundSystem().AddSound("MenuTheme_1.mp3", SoundId::Music, true);
		ServiceLocator::GetSoundSystem().PlaySound(SoundId::Music, ServiceLocator::GetSoundSystem().GetCurrentVolume());
	}

	void MainMenuScene::OnExit()
	{
		gameObjects.clear();
		itemObjects.clear();
		valueObjects.clear();
		items.clear();
	}
}

