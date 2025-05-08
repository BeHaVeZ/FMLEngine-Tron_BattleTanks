#include "InputHandler.h"
#include "XInputGamepadHandlerImpl.h"
#include <iostream>
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "Logger.h"

namespace FML
{
	InputHandler& InputHandler::Instance()
	{
		static InputHandler instance;
		return instance;
	}

	void InputHandler::BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action)
	{
		if (action == KeyAction::KeyDown)
		{
			keyDownCommands[key] = std::move(command);
		}
		else {
			keyUpCommands[key] = std::move(command);
		}
	}

	void InputHandler::BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, KeyAction action)
	{
		gamepadHandler->BindGamepadCommand(controllerId, button, std::move(command), action);
	}

	void InputHandler::BindFunction(SDL_Keycode key, std::function<void()> func, KeyAction action)
	{
		if (action == KeyAction::KeyDown)
			keyDownFunctions[key] = std::move(func);
		else
			keyUpFunctions[key] = std::move(func);
	}

	void InputHandler::UnbindCommand(SDL_Keycode key, KeyAction action)
	{
		if (action == KeyAction::KeyDown)
		{
			auto it = keyDownCommands.find(key);
			if (it != keyDownCommands.end())
				keyDownCommands.erase(it);
		}
		else
		{
			auto it = keyUpCommands.find(key);
			if (it != keyUpCommands.end())
				keyUpCommands.erase(it);
		}
	}

	void InputHandler::UnbindFunction(SDL_Keycode key, KeyAction action)
	{
		if (action == KeyAction::KeyDown)
		{
			auto it = keyDownFunctions.find(key);
			if (it != keyDownFunctions.end())
				keyDownFunctions.erase(it);
		}
		else
		{
			auto it = keyUpFunctions.find(key);
			if (it != keyUpFunctions.end())
				keyUpFunctions.erase(it);
		}
	}

	void InputHandler::BindGamepadFunction(int controllerId, int button, std::function<void()> func, KeyAction action)
	{
		gamepadHandler->BindGamepadFunction(controllerId, button, func, action);
	}

	void InputHandler::UnbindGamepadCommand(int controllerId, int button, KeyAction action)
	{
		gamepadHandler->UnbindGamepadCommand(controllerId, button, action);
	}

	void InputHandler::UnbindGamepadFunction(int controllerId, int button, KeyAction action)
	{
		gamepadHandler->UnbindGamepadFunction(controllerId, button, action);
	}

	void InputHandler::Update()
	{
		for (auto& [key, isPressed] : keyStates)
		{
			if (isPressed) {
				auto it = keyDownCommands.find(key);
				if (it != keyDownCommands.end() && it->second)
				{
					it->second->Execute();
				}
			}
		}
		gamepadHandler->UpdateGamepadStates();
	}

	void InputHandler::ClearBindings()
	{
		keyDownCommands.clear();
		keyUpCommands.clear();
		keyDownFunctions.clear();
		keyUpFunctions.clear();
		gamepadHandler->ClearBindings();
	}

	glm::vec2 firstClick;
	glm::vec2 secondClick;
	bool isFirstClick = true;

	void InputHandler::HandleInput(SDL_Event& event)
	{
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int x = event.button.x;
				int y = event.button.y;

				if (isFirstClick) {
					firstClick = { x, y };
					isFirstClick = false;
					std::cout << "First click at: (" << x << ", " << y << ")" << std::endl;
				}
				else {
					secondClick = { x, y };
					isFirstClick = true;
					std::cout << "Second click at: (" << x << ", " << y << ")" << std::endl;
					int width = (int)secondClick.x - (int)firstClick.x;
					int height = (int)secondClick.y - (int)firstClick.y;
					std::cout << "Rectangle dimensions: {" << firstClick.x << ", " << firstClick.y << ", " << width << ", " << height << "}," << std::endl;
				}
			}
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				int x = event.button.x;
				int y = event.button.y;
				glm::vec2 clickPos = { x, y };

				Logger::Log(LogLevel::Info, "Spawning tank at: (%d, %d)", x, y);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(PrefabRegistry::Instance().CreateRecognizerPrefab(clickPos));
			}
			if (event.button.button == SDL_BUTTON_MIDDLE)
			{
				int x = event.button.x;
				int y = event.button.y;
				glm::vec2 clickPos = { x, y };

				Logger::Log(LogLevel::Info, "Spawning tank at: (%d, %d)", x, y);
				SceneManager::Instance().GetCurrentScene()->AddGameObject(PrefabRegistry::Instance().CreateBlueTankPrefab(clickPos));
			}
		}

		if (event.type == SDL_KEYDOWN)
		{
			if (!event.key.repeat)
			{
				keyStates[event.key.keysym.sym] = true;
				auto it = keyDownCommands.find(event.key.keysym.sym);
				if (it != keyDownCommands.end() && it->second)
					it->second->Execute();

				if (auto ite = keyDownFunctions.find(event.key.keysym.sym); ite != keyDownFunctions.end())
					ite->second();
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			keyStates[event.key.keysym.sym] = false;
			auto it = keyUpCommands.find(event.key.keysym.sym);
			if (it != keyUpCommands.end() && it->second)
				it->second->Execute();

			if (auto ite = keyUpFunctions.find(event.key.keysym.sym); ite != keyUpFunctions.end())
				ite->second();
		}
	}
}

