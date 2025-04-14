#include "SceneManager.h"
#include "SceneManager.h"
#include "InputHandler.h"
#include "ServiceLocator.h"

namespace FML
{

	void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene) {
		scenes[name] = std::move(scene);
		sceneOrder.push_back(name);
	}

	void SceneManager::ChangeScene(const std::string& sceneName) {
		auto it = scenes.find(sceneName);
		if (it != scenes.end()) {
			InputHandler::Instance().ClearBindings();
			ServiceLocator::GetSoundSystem().ClearSounds();

			if (currentScene) 
			{
				currentScene->Cleanup();
			}

			currentScene = it->second.get();
			currentScene->Initialize(localRenderer);
		}
	}

	void SceneManager::RemoveScene(const std::string& name) {
		auto it = scenes.find(name);
		if (it != scenes.end()) {
			if (currentScene == it->second.get()) 
			{
				currentScene->Cleanup();     
				currentScene = nullptr;
			}
			it->second->Cleanup();     
			scenes.erase(it);

			sceneOrder.erase(std::remove(sceneOrder.begin(), sceneOrder.end(), name), sceneOrder.end());
		}
	}

	Scene* SceneManager::GetCurrentScene() const {
		return currentScene;
	}

	void SceneManager::ReloadScene()
	{
		if (!currentScene) return;

		auto currentIt = std::find_if(scenes.begin(), scenes.end(),
			[this](const auto& pair) { return pair.second.get() == currentScene; });

		if (currentIt == scenes.end()) return;

		InputHandler::Instance().ClearBindings();
		ServiceLocator::GetSoundSystem().ClearSounds();

		currentScene->Cleanup();
		currentScene->Initialize(localRenderer);

		std::cout << "Restarted scene: " << currentIt->first << std::endl;
	}

	void SceneManager::GoToNextScene()
	{
		if (!currentScene) return;

		auto currentIt = std::find_if(scenes.begin(), scenes.end(),
			[this](const auto& pair) { return pair.second.get() == currentScene; });

		if (currentIt == scenes.end()) return;

		auto nameIt = std::find(sceneOrder.begin(), sceneOrder.end(), currentIt->first);
		if (nameIt != sceneOrder.end() && (nameIt + 1) != sceneOrder.end()) {
			std::string nextSceneName = *(nameIt + 1);
			ChangeScene(nextSceneName);
		}
		else {
			std::cout << "No next scene. Looping to first.\n";
			ChangeScene(sceneOrder.front());
		}
	}

	void SceneManager::HandleInput(SDL_Event& event) {
		if (currentScene) {
			currentScene->HandleInput(event);
		}
	}

	void SceneManager::Update(float deltaTime) {
		if (currentScene) 
		{
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

