#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include <glm.hpp>
#include "GameObject.h"
#include <string>

namespace FML 
{

    class Scene 
    {
    public:
        Scene(const std::string& name = "") : sceneName(name) {}
        virtual ~Scene() {}
        Scene& operator=(const Scene&) = delete;
        Scene& operator=(Scene&&) = delete;

        const std::string& GetName() const { return sceneName; }

        virtual bool Initialize(SDL_Renderer* renderer) = 0;
        virtual void HandleInput(SDL_Event& event) = 0;
        virtual void InitializeInput() = 0;
        virtual void Update(float deltaTime);
        virtual void Render(SDL_Renderer* renderer);
        virtual void Cleanup();
        virtual void AddGameObject(std::unique_ptr<GameObject> gameObject, glm::vec2 position = { 0,0 });

        GameObject* FindGameObjectByTag(const std::string& tag);
        void CleanupDestroyedGameObjects();

        virtual void OnExit() {};

    protected:
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        const std::string sceneName;

        void UpdateGameObjects(float deltaTime);
        void RenderGameObjects(SDL_Renderer* renderer);
    };
}
