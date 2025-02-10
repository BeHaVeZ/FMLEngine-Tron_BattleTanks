#pragma once
#include <map>
#include <string>
#include <memory>
#include "Scene.h"

class SceneManager {
public:
    static SceneManager& Instance() {
        static SceneManager instance;
        return instance;
    }

    void AddScene(const std::string& name, std::unique_ptr<Scene> scene);
    void ChangeScene(const std::string& sceneName, SDL_Renderer* renderer);
    void RemoveScene(const std::string& name);
    Scene* GetCurrentScene() const;

    void HandleInput(SDL_Event& event);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

private:
    SceneManager() = default;

    std::map<std::string, std::unique_ptr<Scene>> scenes;
    Scene* currentScene = nullptr;
};
