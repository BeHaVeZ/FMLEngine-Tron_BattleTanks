#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include <glm.hpp>
#include "GameObject.h"

namespace FML 
{

    class Scene 
    {
    public:
        virtual ~Scene() {}

        virtual bool Initialize(SDL_Renderer* renderer) = 0;
        virtual void HandleInput(SDL_Event& event) = 0;
        virtual void InitializeInput() = 0;
        virtual void Update(float deltaTime);
        virtual void Render(SDL_Renderer* renderer);
        virtual void Cleanup() = 0;
        virtual void AddGameObject(std::unique_ptr<GameObject> gameObject, glm::vec2 position = { 0,0 });

        GameObject* FindGameObjectByTag(const std::string& tag);

    protected:
        std::vector<std::unique_ptr<GameObject>> gameObjects;

        void UpdateGameObjects(float deltaTime);
        void RenderGameObjects(SDL_Renderer* renderer);
    };
}
