#include "InputHandler.h"
#include "XInputGamepadHandlerImpl.h"
#include <iostream>

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

void InputHandler::Update() 
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
    gamepadHandler->UpdateGamepadStates();
}

void InputHandler::ClearBindings() 
{
    keyDownCommands.clear();
    keyUpCommands.clear();
    gamepadHandler->ClearBindings();
}

void InputHandler::HandleInput(SDL_Event& event) 
{
    if (event.type == SDL_KEYDOWN) 
    {
        if (!event.key.repeat) 
        {
            keyStates[event.key.keysym.sym] = true;
            auto it = keyDownCommands.find(event.key.keysym.sym);
            if (it != keyDownCommands.end() && it->second) 
            {
                it->second->Execute();
            }
        }
    }
    else if (event.type == SDL_KEYUP) 
    {
        keyStates[event.key.keysym.sym] = false;
        auto it = keyUpCommands.find(event.key.keysym.sym);
        if (it != keyUpCommands.end() && it->second) 
        {
            it->second->Execute();
        }
    }
}
