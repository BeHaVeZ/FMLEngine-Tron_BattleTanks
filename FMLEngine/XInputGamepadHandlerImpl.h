#pragma once
#include <Windows.h>
#include <XInput.h>
#include <map>
#include <memory>
#include <functional>
#include "Command.h"
#include "InputHandler.h"

namespace FML
{

	class XInputGamepadHandlerImpl final
	{
	public:
		void BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, InputHandler::KeyAction action);
		void BindGamepadFunction(int controllerId, int button, std::function<void()>, InputHandler::KeyAction action);
		void UnbindGamepadCommand(int controllerId, int button, InputHandler::KeyAction action);
		void UnbindGamepadFunction(int controllerId, int button, InputHandler::KeyAction action);
		void UpdateGamepadStates();
		void ClearBindings();
		void SetCommandsSuspended(bool suspended) { commandsSuspended = suspended; }

	private:
		void UpdateSingleGamepadState(DWORD dwUserIndex);
		XINPUT_STATE gamepadStates[XUSER_MAX_COUNT];
		struct GamepadCommands 
		{
			std::map<int, std::unique_ptr<Command>> downCommands;
			std::map<int, std::unique_ptr<Command>> upCommands;
		};

		struct GamepadFunctions
		{
			std::map<int, std::function<void()>> downFunctions;
			std::map<int, std::function<void()>> upFunctions;
		};;

		std::map<int, GamepadCommands> gamepadCommands;
		std::map<int, GamepadFunctions> gamepadFunctions;
		bool commandsCleared = false;
		bool commandsSuspended = false;
	};

}


