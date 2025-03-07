#pragma once
#include <SDL.h>
#include <map>
#include <memory>
#include "Command.h"
#include <Windows.h>
#include <list>
#include "Xinput.h"

class InputHandler 
{
public:
    static InputHandler& Instance() 
    {
        static InputHandler instance;
        return instance;
    }

    enum class KeyAction 
    {
        KeyDown,
        KeyUp
    };

    void BindCommand(SDL_Keycode key, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);
    void BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, KeyAction action = KeyAction::KeyDown);

    void Update();
    void UpdateGamepadStates();
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
    std::list<SDL_Keycode> movementKeys;



    XINPUT_STATE gamepadStates[XUSER_MAX_COUNT];
    bool gamepadButtonStates[XUSER_MAX_COUNT][XINPUT_KEYSTROKE_KEYDOWN];
    void UpdateSingleGamepadState(DWORD dwUserIndex);
    struct GamepadCommands 
    {
        std::map<int, std::unique_ptr<Command>> downCommands;
        std::map<int, std::unique_ptr<Command>> upCommands;
    };
    std::map<int, GamepadCommands> gamepadCommands;
};