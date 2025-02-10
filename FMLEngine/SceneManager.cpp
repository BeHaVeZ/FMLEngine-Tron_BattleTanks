#include "SceneManager.h"
#include "InputHandler.h"

void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene) {
    scenes[name] = std::move(scene);
}

void SceneManager::ChangeScene(const std::string& sceneName, SDL_Renderer* renderer) {
    auto it = scenes.find(sceneName);
    if (it != scenes.end()) {
        InputHandler::Instance().ClearBindings();

        if (currentScene) {
            currentScene->Cleanup();
        }

        currentScene = it->second.get();
        currentScene->Initialize(renderer);
    }
}

void SceneManager::RemoveScene(const std::string& name) {
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        if (currentScene == it->second.get()) {
            currentScene = nullptr;
        }
        scenes.erase(it);
    }
}

Scene* SceneManager::GetCurrentScene() const {
    return currentScene;
}

void SceneManager::HandleInput(SDL_Event& event) {
    if (currentScene) {
        currentScene->HandleInput(event);
    }
}

void SceneManager::Update(float deltaTime) {
    if (currentScene) {
        currentScene->Update(deltaTime);
    }
}

void SceneManager::Render(SDL_Renderer* renderer) {
    if (currentScene) {
        currentScene->Render(renderer);
    }
}
