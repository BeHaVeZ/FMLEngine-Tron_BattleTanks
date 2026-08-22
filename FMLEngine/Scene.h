#pragma once
#include <SDL.h>
#include <vector>
#include <memory>
#include <glm.hpp>
#include "GameObject.h"
#include <string>
#include <string_view>

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
		void AddGameObject(std::unique_ptr<GameObject> gameObject);
		void AddGameObject(std::unique_ptr<GameObject> gameObject, const glm::vec2& position);

        GameObject* FindGameObjectByTag(std::string_view tag);
        std::vector<GameObject*> FindGameObjectsByTag(std::string_view tag) const;
        void FindGameObjectsByTag(std::string_view tag, std::vector<GameObject*>& outFound) const;

        template <typename Fn>
        void ForEachGameObject(Fn&& fn) const
        {
            for (const auto& gameObject : gameObjects)
            {
                if (!gameObject->IsMarkedForDestruction())
                {
                    fn(*gameObject);
                }
            }
        }

        void CleanupDestroyedGameObjects();

        virtual void OnEnter();
        virtual void OnExit();

    protected:
        std::vector<std::unique_ptr<GameObject>> gameObjects;
        const std::string sceneName;

        void UpdateGameObjects(float deltaTime);
        void RenderGameObjects(SDL_Renderer* renderer);
    };
}
