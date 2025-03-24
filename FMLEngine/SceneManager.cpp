#include "SceneManager.h"
#include "InputHandler.h"
#include "ServiceLocator.h"

namespace FML
{

	void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene) {
		scenes[name] = std::move(scene);
	}

	void SceneManager::ChangeScene(const std::string& sceneName) {
		auto it = scenes.find(sceneName);
		if (it != scenes.end()) {
			InputHandler::Instance().ClearBindings();
			ServiceLocator::GetSoundSystem().ClearSounds();

			if (currentScene) {
				currentScene->Cleanup();
			}

			currentScene = it->second.get();
			currentScene->Initialize(localRenderer);
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

	void SceneManager::Render() {
		if (currentScene)
		{
			currentScene->Render(localRenderer);
		}
	}

	void SceneManager::SetRenderer(SDL_Renderer* newRenderer)
	{
		if (newRenderer)
		{
			localRenderer = newRenderer;
		}
	}

	SDL_Renderer* SceneManager::GetRenderer() const
	{
		if (localRenderer)
		{
			return localRenderer;
		}
		return nullptr;
	}

}

