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

    void BindCommand(SDL_Keycode key, std::unique_ptr<Command> command);
    void ClearBindings();
    void HandleInput(SDL_Event& event);

    InputHandler(const InputHandler&) = delete;
    InputHandler& operator=(const InputHandler&) = delete;

private:
    InputHandler() = default;
    std::map<SDL_Keycode, std::unique_ptr<Command>> keyCommandMap;
};