#pragma once
#include "SDL.h"

class GameObject;

class Component {
public:
    virtual ~Component() = default;

    virtual void Initialize() {};
    virtual void Update(float deltaTime) {};
    virtual void Render(SDL_Renderer* renderer) {};

    void SetOwner(GameObject* owner) {
        this->gameObject = owner;
    }

protected:
    GameObject* gameObject = nullptr;
};
