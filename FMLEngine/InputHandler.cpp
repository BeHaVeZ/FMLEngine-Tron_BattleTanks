#include "InputHandler.h"
#include <Windows.h>
#include "Xinput.h"

void InputHandler::BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action) {
    if (action == KeyAction::KeyDown) {
        keyDownCommands[key] = std::move(command);
    }
    else {
        keyUpCommands[key] = std::move(command);
    }
}


void InputHandler::ClearBindings() {
    keyDownCommands.clear();
    keyUpCommands.clear();
}

void InputHandler::HandleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        keyStates[event.key.keysym.sym] = true;
        auto it = keyDownCommands.find(event.key.keysym.sym);
        if (it != keyDownCommands.end() && it->second) {
            it->second->Execute();
        }
    }
    else if (event.type == SDL_KEYUP) {
        keyStates[event.key.keysym.sym] = false;
        auto it = keyUpCommands.find(event.key.keysym.sym);
        if (it != keyUpCommands.end() && it->second) {
            it->second->Execute();
        }
    }
}

bool InputHandler::IsKeyPressed(SDL_Keycode key) const
{
    auto it = keyStates.find(key);
    return it != keyStates.end() ? it->second : false;
}
