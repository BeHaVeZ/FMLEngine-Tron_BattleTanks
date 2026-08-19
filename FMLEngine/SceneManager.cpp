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
		sceneChangeDelay = 0.f;
	}

	void SceneManager::QueueSceneChangeWithDelay(const std::string& sceneName, float delaySeconds)
	{
		queuedSceneChange = sceneName;
		sceneChangeDelay = delaySeconds;
	}

	Scene* SceneManager::GetCurrentScene() const {
		return currentScene;
	}

	const std::string& SceneManager::GetSceneName() const
	{
		static const std::string emptyName = "";

		if (!currentScene)
			return emptyName;

		return currentScene->GetName();
	}

	Scene* SceneManager::GetNextScene() const
	{
		if (!currentScene) return nullptr;

		auto nameIt = std::find(sceneOrder.begin(), sceneOrder.end(), currentScene->GetName());
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

	void SceneManager::ReloadScene()
	{
		if (!currentScene) return;

		std::cout << "Restarting scene: " << currentScene->GetName() << std::endl;
		QueueSceneChange(currentScene->GetName());
	}

	void SceneManager::GoToNextScene()
	{
		if (!currentScene) return;

		auto nameIt = std::find(sceneOrder.begin(), sceneOrder.end(), currentScene->GetName());
		if (nameIt != sceneOrder.end() && (nameIt + 1) != sceneOrder.end()) {
			std::string nextSceneName = *(nameIt + 1);
			QueueSceneChange(nextSceneName);
		}
		else {
			std::cout << "No next scene. Looping to first.\n";
			QueueSceneChange(sceneOrder.front());
		}
	}

	void SceneManager::Update(float deltaTime) 
	{
		isInsideSceneUpdate = true;
		if (currentScene) currentScene->Update(deltaTime);
		isInsideSceneUpdate = false;

		if (!queuedSceneChange.empty())
		{
			sceneChangeDelay -= deltaTime;
			if (sceneChangeDelay <= 0.f)
			{
				ChangeScene(queuedSceneChange);
				queuedSceneChange.clear();
				sceneChangeDelay = 0.f;
			}
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
		return localRenderer;
	}

}
