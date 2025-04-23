#include "XInputGamepadHandlerImpl.h"
#include "InputHandler.h"
#include <iostream>

namespace FML
{

	void XInputGamepadHandlerImpl::BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, InputHandler::KeyAction action = InputHandler::KeyAction::KeyDown)
	{
		if (action == InputHandler::KeyAction::KeyDown)
		{
			gamepadCommands[controllerId].downCommands[button] = std::move(command);
		}
		else
		{
			gamepadCommands[controllerId].upCommands[button] = std::move(command);
		}
	}

	void XInputGamepadHandlerImpl::BindGamepadFunction(int controllerId, int button, std::function<void()> func, InputHandler::KeyAction action)
	{
		if (action == InputHandler::KeyAction::KeyDown)
		{
			gamepadFunctions[controllerId].downFunctions[button] = std::move(func);
		}
		else
		{
			gamepadFunctions[controllerId].upFunctions[button] = std::move(func);
		}
	}

	void XInputGamepadHandlerImpl::UnbindGamepadCommand(int controllerId, int button, InputHandler::KeyAction action)
	{
		if (action == InputHandler::KeyAction::KeyDown)
		{
			auto& map = gamepadCommands[controllerId].downCommands;
			if (map.count(button)) map.erase(button);
		}
		else
		{
			auto& map = gamepadCommands[controllerId].upCommands;
			if (map.count(button)) map.erase(button);
		}
	}

	void XInputGamepadHandlerImpl::UnbindGamepadFunction(int controllerId, int button, InputHandler::KeyAction action)
	{
		if (action == InputHandler::KeyAction::KeyDown)
		{
			auto& map = gamepadFunctions[controllerId].downFunctions;
			if (map.count(button)) map.erase(button);
		}
		else
		{
			auto& map = gamepadFunctions[controllerId].upFunctions;
			if (map.count(button)) map.erase(button);
		}
	}

	void XInputGamepadHandlerImpl::UpdateGamepadStates()
	{
		commandsCleared = false;
		for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
			UpdateSingleGamepadState(i);
		}
	}

	void XInputGamepadHandlerImpl::ClearBindings()
	{
		commandsCleared = true;
		for (auto& [controllerId, commands] : gamepadCommands)
		{
			commands.downCommands.clear();
			commands.upCommands.clear();
		}

		for (auto& [controllerId, functions] : gamepadFunctions)
		{
			functions.downFunctions.clear();
			functions.upFunctions.clear();
		}
		for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			ZeroMemory(&gamepadStates[i], sizeof(XINPUT_STATE));
		}
	}

	void XInputGamepadHandlerImpl::UpdateSingleGamepadState(DWORD dwUserIndex)
	{
		XINPUT_STATE newState;
		ZeroMemory(&newState, sizeof(XINPUT_STATE));

		if (XInputGetState(dwUserIndex, &newState) == ERROR_SUCCESS)
		{
			auto& commands = gamepadCommands[dwUserIndex];
			auto& functions = gamepadFunctions[dwUserIndex];

			for (const auto& [button, command] : commands.downCommands)
			{
				bool isPressed = (newState.Gamepad.wButtons & button) != 0;
				if (isPressed && command)
				{
					command->Execute();
					if (commandsCleared) return;
				}
			}

			for (const auto& [button, command] : commands.upCommands)
			{
				bool wasPressed = (gamepadStates[dwUserIndex].Gamepad.wButtons & button) != 0;
				bool isPressed = (newState.Gamepad.wButtons & button) != 0;

				if (wasPressed && !isPressed && command)
				{
					command->Execute();
					if (commandsCleared) return;
				}
			}

			for (const auto& [button, func] : functions.downFunctions)
			{
				bool isPressed = (newState.Gamepad.wButtons & button) != 0;
				if (isPressed && func)
				{
					func();
					if (commandsCleared) return;
				}
			}

			for (const auto& [button, func] : functions.upFunctions)
			{
				bool wasPressed = (gamepadStates[dwUserIndex].Gamepad.wButtons & button) != 0;
				bool isPressed = (newState.Gamepad.wButtons & button) != 0;

				if (wasPressed && !isPressed && func)
				{
					func();
					if (commandsCleared) return;
				}
			}

			gamepadStates[dwUserIndex] = newState;
		}
	}

}

