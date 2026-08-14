#pragma once
#include <map>
#include <string>
#include <memory>
#include "Scene.h"

namespace FML
{

	class SceneManager {
	public:
		static SceneManager& Instance() 
		{
			static SceneManager instance;
			return instance;
		}

		void AddScene(std::unique_ptr<Scene> scene);
		void ChangeScene(const std::string& sceneName);
		void QueueSceneChange(const std::string& name);
		void QueueSceneChangeWithDelay(const std::string& sceneName, float delaySeconds);
		const std::string& GetSceneName() const;
		Scene* GetCurrentScene() const;
		Scene* GetNextScene() const;
		void GoToNextScene();
		void ReloadScene();

		void Update(float deltaTime);
		void Render();

		SceneManager(const SceneManager&) = delete;
		SceneManager& operator=(const SceneManager&) = delete;

		void SetRenderer(SDL_Renderer* newRenderer);
		SDL_Renderer* GetRenderer() const;

	private:
		SceneManager() : localRenderer(nullptr), currentScene(nullptr),isInsideSceneUpdate(false),queuedSceneChange(""),sceneChangeDelay(0.f) {}

		std::map<std::string, std::unique_ptr<Scene>> scenes;
		std::string queuedSceneChange;
		std::vector<std::string> sceneOrder;
		Scene* currentScene = nullptr;

		float sceneChangeDelay;

		bool isInsideSceneUpdate;

		SDL_Renderer* localRenderer = nullptr;
	};


}

