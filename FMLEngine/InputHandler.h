#pragma once
#include <SDL.h>
#include <map>
#include <memory>
#include "Command.h"

class InputHandler {
public:
    static InputHandler& Instance() {
        static InputHandler instance;
        return instance;
    }

    enum class KeyAction {
        KeyDown,
        KeyUp
    };

    void BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);

    void ClearBindings();
    void HandleInput(SDL_Event& event);
    bool IsKeyPressed(SDL_Keycode key) const;


    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

private:
    InputHandler() = default;
    std::map<SDL_Keycode, bool> keyStates;
    std::map<SDL_Keycode, std::unique_ptr<Command>> keyDownCommands;
    std::map<SDL_Keycode, std::unique_ptr<Command>> keyUpCommands;
};