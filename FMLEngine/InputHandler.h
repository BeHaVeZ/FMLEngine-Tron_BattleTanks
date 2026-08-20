#pragma once
#include <SDL.h>
#include <map>
#include <memory>
#include <functional>
#include "Command.h"
#include "Windows.h"
#include "Xinput.h"

namespace FML
{

	class XInputGamepadHandlerImpl;

	class InputHandler {
	public:
		static InputHandler& Instance();

		enum class KeyAction {
			KeyDown,
			KeyUp
		};

		void BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);
		void BindFunction(SDL_Keycode key, std::function<void()> func, KeyAction action = KeyAction::KeyDown);
		void UnbindCommand(SDL_Keycode key, KeyAction action);
		void UnbindFunction(SDL_Keycode key, KeyAction action);

		void BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);
		void BindGamepadFunction(int controllerId,int button, std::function<void()> func, KeyAction action = KeyAction::KeyDown);
		void UnbindGamepadCommand(int controllerId, int button, KeyAction action);
		void UnbindGamepadFunction(int controllerId, int button, KeyAction action);

		void Update();
		void ClearBindings();

		void SetCommandsSuspended(bool suspended);
		bool AreCommandsSuspended() const { return commandsSuspended; }

		void HandleInput(SDL_Event& event);

		InputHandler(const InputHandler&) = delete;
		InputHandler& operator=(const InputHandler&) = delete;

	private:
		InputHandler() = default;
		~InputHandler() = default;

		bool commandsSuspended = false;

		std::map<SDL_Keycode, bool> keyStates;
		std::map<SDL_Keycode, std::unique_ptr<Command>> keyDownCommands;
		std::map<SDL_Keycode, std::unique_ptr<Command>> keyUpCommands;

		std::map<SDL_Keycode, std::function<void()>> keyDownFunctions;
		std::map<SDL_Keycode, std::function<void()>> keyUpFunctions;

		std::unique_ptr<XInputGamepadHandlerImpl> gamepadHandler{ std::make_unique<XInputGamepadHandlerImpl>() };
	};

}

