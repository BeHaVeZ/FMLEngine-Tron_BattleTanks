#include "InputHandler.h"
#include "XInputGamepadHandlerImpl.h"

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
		if (!commandsSuspended)
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
		}
		gamepadHandler->UpdateGamepadStates();
	}

	void InputHandler::SetCommandsSuspended(bool suspended)
	{
		commandsSuspended = suspended;
		gamepadHandler->SetCommandsSuspended(suspended);
	}

	void InputHandler::ClearBindings()
	{
		commandsSuspended = false;
		keyDownCommands.clear();
		keyUpCommands.clear();
		keyDownFunctions.clear();
		keyUpFunctions.clear();
		keyStates.clear();
		gamepadHandler->ClearBindings();
	}

	void InputHandler::HandleInput(SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			if (!event.key.repeat)
			{
				keyStates[event.key.keysym.sym] = true;
				if (auto ite = keyDownFunctions.find(event.key.keysym.sym); ite != keyDownFunctions.end())
					ite->second();
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			keyStates[event.key.keysym.sym] = false;

			if (!commandsSuspended)
			{
				auto it = keyUpCommands.find(event.key.keysym.sym);
				if (it != keyUpCommands.end() && it->second)
					it->second->Execute();
			}

			if (auto ite = keyUpFunctions.find(event.key.keysym.sym); ite != keyUpFunctions.end())
				ite->second();
		}
	}
}

