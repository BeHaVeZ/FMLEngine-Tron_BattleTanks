#include "InputHandler.h"

void InputHandler::BindCommand(SDL_Keycode key, std::unique_ptr<Command> command) {
    keyCommandMap[key] = std::move(command);
}

void InputHandler::ClearBindings() {
    keyCommandMap.clear();
}

void InputHandler::HandleInput(SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        auto it = keyCommandMap.find(event.key.keysym.sym);
        if (it != keyCommandMap.end() and it->second) {
            it->second->Execute();
        }
    }
    else if (event.type == SDL_KEYUP) {
        auto it = keyCommandMap.find(event.key.keysym.sym);
        if (it != keyCommandMap.end() and it->second) {
            it->second->Release();
        }
    }

    // IS TO BE REMOVED
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);

    if (currentKeyStates[SDL_SCANCODE_LEFT]) {
        auto it = keyCommandMap.find(SDLK_LEFT);
        if (it != keyCommandMap.end() and it->second) {
            it->second->Execute();
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_RIGHT]) {
        auto it = keyCommandMap.find(SDLK_RIGHT);
        if (it != keyCommandMap.end() and it->second) {
            it->second->Execute();
        }
    }
    else {
        if (keyCommandMap[SDLK_LEFT] and keyCommandMap[SDLK_LEFT].get()) {
            keyCommandMap[SDLK_LEFT]->Release();
        }
        if (keyCommandMap[SDLK_RIGHT] and keyCommandMap[SDLK_RIGHT].get()) {
            keyCommandMap[SDLK_RIGHT]->Release();
        }
    }
}
