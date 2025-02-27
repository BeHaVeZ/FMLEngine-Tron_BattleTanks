#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include "GameObject.h"

class Scene {
public:
    virtual ~Scene() {}

    virtual bool Initialize(SDL_Renderer* renderer) = 0;
    virtual void HandleInput(SDL_Event& event) = 0;
    virtual void InitializeInput() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
    virtual void Cleanup() = 0;


    GameObject* FindGameObjectByTag(const std::string& tag);

protected:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};
