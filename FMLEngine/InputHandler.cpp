#include "InputHandler.h"
#include <iostream>

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
	if (action == KeyAction::KeyDown)
	{
		gamepadCommands[controllerId].downCommands[button] = std::move(command);
	}
	else
	{
		gamepadCommands[controllerId].upCommands[button] = std::move(command);
	}
}


void InputHandler::Update()
{
	if (!movementKeys.empty()) {
		auto lastKey = movementKeys.back(); 
		if (keyStates[lastKey]) {  
			auto it = keyDownCommands.find(lastKey);
			if (it != keyDownCommands.end()) {
				it->second->Execute();
			}
		}
	}
	UpdateGamepadStates();
}

void InputHandler::UpdateGamepadStates()
{
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) 
	{
		UpdateSingleGamepadState(i);
	}
}

void InputHandler::ClearBindings() {
	keyDownCommands.clear();
	keyUpCommands.clear();

	for (auto& [controllerId, commands] : gamepadCommands) 
	{
		commands.downCommands.clear();
		commands.upCommands.clear();
	}
	for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
		ZeroMemory(&gamepadStates[i], sizeof(XINPUT_STATE));
	}
}

void InputHandler::HandleInput(SDL_Event& event)
{
	if (event.type == SDL_KEYDOWN && !event.key.repeat) {
		if (movementKeys.size() == 2) {
			movementKeys.remove(event.key.keysym.sym); 
		}
		movementKeys.push_back(event.key.keysym.sym);
		keyStates[event.key.keysym.sym] = true;
		auto it = keyDownCommands.find(event.key.keysym.sym);
		if (it != keyDownCommands.end()) {
			it->second->Execute();
		}
	}
	else if (event.type == SDL_KEYUP) {
		keyStates[event.key.keysym.sym] = false;
		movementKeys.remove(event.key.keysym.sym);
		auto it = keyUpCommands.find(event.key.keysym.sym);
		if (it != keyUpCommands.end()) {
			it->second->Execute();
		}
	}
}

bool InputHandler::IsKeyPressed(SDL_Keycode key) const
{
	auto it = keyStates.find(key);
	return it != keyStates.end() ? it->second : false;
}

void InputHandler::UpdateSingleGamepadState(DWORD dwUserIndex) {
	XINPUT_STATE newState;
	ZeroMemory(&newState, sizeof(XINPUT_STATE));
	if (XInputGetState(dwUserIndex, &newState) == ERROR_SUCCESS) {
		auto& commands = gamepadCommands[dwUserIndex];
		bool continueProcessing = true;

		for (const auto& [button, command] : commands.downCommands) 
		{
			bool isPressed = (newState.Gamepad.wButtons & button) != 0;
			if (isPressed) {
				command->Execute();
				if (commands.downCommands.empty() || commands.upCommands.empty()) {
					continueProcessing = false;
					break;
				}
			}
		}

		if (!continueProcessing) return;
		for (const auto& [button, command] : commands.upCommands) {
			bool wasPressed = (gamepadStates[dwUserIndex].Gamepad.wButtons & button) != 0;
			bool isPressed = (newState.Gamepad.wButtons & button) != 0;
			if (wasPressed && !isPressed) {
				command->Execute();
				if (commands.downCommands.empty() || commands.upCommands.empty()) {
					break;
				}
			}
		}

		gamepadStates[dwUserIndex] = newState;
	}
}