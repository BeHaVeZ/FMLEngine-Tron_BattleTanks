#include "Scene.h"

GameObject* Scene::FindGameObjectByTag(const std::string& tag)
{
    for (const auto& gameObject : gameObjects) {
        if (gameObject->GetTag() == tag) {
            return gameObject.get();
        }
    }
    return nullptr;
}
