#pragma once
#include <SDL.h>
#include <map>
#include <memory>
#include "Command.h"
#include "Windows.h"
#include "Xinput.h"

class XInputGamepadHandlerImpl;

class InputHandler {
public:
    static InputHandler& Instance();

    enum class KeyAction {
        KeyDown,
        KeyUp
    };

    void BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);
    void BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);

    void Update();
    void ClearBindings();
    void HandleInput(SDL_Event& event);

    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

private:
    InputHandler() = default;
    ~InputHandler() = default;
    std::map<SDL_Keycode, bool> keyStates;
    std::map<SDL_Keycode, std::unique_ptr<Command>> keyDownCommands;
    std::map<SDL_Keycode, std::unique_ptr<Command>> keyUpCommands;

    std::unique_ptr<XInputGamepadHandlerImpl> gamepadHandler{ std::make_unique<XInputGamepadHandlerImpl>() };
};
