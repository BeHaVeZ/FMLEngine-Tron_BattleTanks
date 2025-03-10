#include "XInputGamepadHandlerImpl.h"
#include "InputHandler.h"
#include <iostream>

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

		for (const auto& [button, command] : commands.downCommands)
		{
			bool isPressed = (newState.Gamepad.wButtons & button) != 0;

			if (isPressed)
			{
				command->Execute();
				if (commandsCleared)
				{
					break;
				}
			}
		}

		for (const auto& [button, command] : commands.upCommands)
		{
			bool wasPressed = (gamepadStates[dwUserIndex].Gamepad.wButtons & button) != 0;
			bool isPressed = (newState.Gamepad.wButtons & button) != 0;

			if (wasPressed && !isPressed)
			{
				command->Execute();
				if (commandsCleared)
				{
					break;
				}
			}
		}
		gamepadStates[dwUserIndex] = newState;
	}
}
