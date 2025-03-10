#pragma once
#include <Windows.h>
#include <XInput.h>
#include <map>
#include <memory>
#include "Command.h"
#include "InputHandler.h"

class XInputGamepadHandlerImpl final
{
public:
    void BindGamepadCommand(int controllerId, int button, std::unique_ptr<Command> command, InputHandler::KeyAction action);
    void UpdateGamepadStates();
    void ClearBindings();

private:
    void UpdateSingleGamepadState(DWORD dwUserIndex);
    XINPUT_STATE gamepadStates[XUSER_MAX_COUNT];
    struct GamepadCommands {
        std::map<int, std::unique_ptr<Command>> downCommands;
        std::map<int, std::unique_ptr<Command>> upCommands;
    };
    std::map<int, GamepadCommands> gamepadCommands;
    bool commandsCleared = false;
};
