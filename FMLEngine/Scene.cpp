#include "Scene.h"

namespace FML 
{

    void Scene::Update(float deltaTime) 
    {
        UpdateGameObjects(deltaTime);
    }

    void Scene::Render(SDL_Renderer* renderer)
    {
        RenderGameObjects(renderer);
    }

    void Scene::UpdateGameObjects(float deltaTime) 
    {
        for (auto it = gameObjects.begin(); it != gameObjects.end();) 
        {
            if ((*it)->IsMarkedForDestruction()) 
            {
                it = gameObjects.erase(it);
            }
            else 
            {
                (*it)->Update(deltaTime);
                ++it;
            }
        }
    }

    void Scene::RenderGameObjects(SDL_Renderer* renderer)
    {
        for (auto& gameObject : gameObjects)
        {
            gameObject->Render(renderer);
        }
    }

    GameObject* Scene::FindGameObjectByTag(const std::string& tag) 
    {
        for (auto& gameObject : gameObjects) 
        {
            if (gameObject->GetTag() == tag) 
            {
                return gameObject.get();
            }
        }
        return nullptr;
    }
}