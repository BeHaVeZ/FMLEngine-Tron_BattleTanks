#include "SceneManager.h"
#include "InputHandler.h"
#include "ServiceLocator.h"

namespace FML
{

	void SceneManager::AddScene(std::unique_ptr<Scene> scene) {
		const std::string& name = scene->GetName();

		if (scenes.count(name) > 0) 
		{
			std::cerr << "[SceneManager] Scene with name '" << name << "' already exists.\n";
			return;
		}

		sceneOrder.push_back(name);
		scenes[name] = std::move(scene);
	}

	void SceneManager::ChangeScene(const std::string& sceneName) {
		assert(!isInsideSceneUpdate && "You must queue scene changes inside Update()");
		auto it = scenes.find(sceneName);
		if (it != scenes.end()) {
			InputHandler::Instance().ClearBindings();
			ServiceLocator::GetSoundSystem().ClearSounds();
			ServiceLocator::GetSoundSystem().ClearQueue();

			if (currentScene) 
			{
				currentScene->OnExit();
				currentScene->Cleanup();
			}

			currentScene = it->second.get();
			currentScene->Initialize(localRenderer);
		}
	}

	void SceneManager::QueueSceneChange(const std::string& name) 
	{
		queuedSceneChange = name;
	}

	void SceneManager::QueueSceneChangeWithDelay(const std::string& sceneName, float delaySeconds)
	{
		queuedSceneChange = sceneName;
		sceneChangeDelay = delaySeconds;
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

	const std::string& SceneManager::GetSceneName() const
	{
		static const std::string emptyName = "";

		if (!currentScene)
			return emptyName;

		auto it = std::find_if(scenes.begin(), scenes.end(),
			[this](const auto& pair) {
				return pair.second.get() == currentScene;
			});

		if (it != scenes.end())
			return it->first;

		return emptyName;
	}

	Scene* SceneManager::GetNextScene() const
	{
		if (!currentScene) return nullptr;

		auto currentIt = std::find_if(scenes.begin(), scenes.end(),
			[this](const auto& pair) {
				return pair.second.get() == currentScene;
			});

		if (currentIt == scenes.end()) 
		{
			std::cerr << "[SceneManager] GetNextScene: Current scene not found in map.\n";
			return nullptr;
		}

		auto nameIt = std::find(sceneOrder.begin(), sceneOrder.end(), currentIt->first);
		if (nameIt != sceneOrder.end() && (nameIt + 1) != sceneOrder.end())
		{
			const std::string& nextSceneName = *(nameIt + 1);
			auto nextIt = scenes.find(nextSceneName);
			if (nextIt != scenes.end())
			{
				return nextIt->second.get();
			}
			else 
			{
				std::cerr << "[SceneManager] GetNextScene: Scene '" << nextSceneName << "' not found in scene map.\n";
			}
		}
		else 
		{
			std::cout << "[SceneManager] GetNextScene: No next scene in order.\n";
		}

		return nullptr;
	}

	Scene* SceneManager::GetPreviousScene() const
	{
		if (!currentScene)
			return nullptr;

		auto currentIt = std::find_if(scenes.begin(), scenes.end(),
			[this](const auto& pair) {
				return pair.second.get() == currentScene;
			});

		if (currentIt == scenes.end())
			return nullptr;

		auto nameIt = std::find(sceneOrder.begin(), sceneOrder.end(), currentIt->first);
		if (nameIt != sceneOrder.begin() && nameIt != sceneOrder.end())
		{
			std::string previousSceneName = *(nameIt - 1);
			auto sceneIt = scenes.find(previousSceneName);
			if (sceneIt != scenes.end())
				return sceneIt->second.get();
		}

		return nullptr;
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
			QueueSceneChange(nextSceneName);
		}
		else {
			std::cout << "No next scene. Looping to first.\n";
			QueueSceneChange(sceneOrder.front());
		}
	}

	void SceneManager::HandleInput(SDL_Event& event) {
		if (currentScene) {
			currentScene->HandleInput(event);
		}
	}

	void SceneManager::Update(float deltaTime) 
	{
		isInsideSceneUpdate = true;
		if (currentScene) currentScene->Update(deltaTime);
		isInsideSceneUpdate = false;

		sceneChangeDelay -= deltaTime;
		if (!queuedSceneChange.empty() && sceneChangeDelay <= 0) 
		{
			ChangeScene(queuedSceneChange);
			queuedSceneChange.clear();
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

