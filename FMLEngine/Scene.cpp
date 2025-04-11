#include "Scene.h"
#include "TransformComponent.h"
#include "CollisionManager.h"
#include "InputHandler.h"

namespace FML
{
	void Scene::Update(float deltaTime)
	{
		InputHandler::Instance().Update();
		UpdateGameObjects(deltaTime);
		CollisionManager::Instance().CheckCollisions();
		CleanupDestroyedGameObjects();
	}

	void Scene::Render(SDL_Renderer* renderer)
	{
		RenderGameObjects(renderer);
	}

	void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject, glm::vec2 position)
	{
		if (gameObject)
		{
			TransformComponent* transform = gameObject->GetComponent<TransformComponent>();

			gameObjects.emplace_back(std::move(gameObject));

			if (transform && position != glm::vec2(0, 0))
			{
				transform->SetPosition(position);
			}
		}
	}

	void Scene::UpdateGameObjects(float deltaTime)
	{
		for (auto& gameObject : gameObjects)
		{
			if (!gameObject->IsMarkedForDestruction())
			{
				gameObject->Update(deltaTime);
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

	void Scene::CleanupDestroyedGameObjects()
	{
		for (auto& gameObject : gameObjects)
		{
			if (gameObject->IsMarkedForDestruction())
			{
				auto collider = gameObject->GetComponent<Collider>();
				if (collider)
					CollisionManager::Instance().UnregisterCollider(collider);
			}
		}

		gameObjects.erase(
			std::remove_if(gameObjects.begin(), gameObjects.end(),
				[](const std::unique_ptr<GameObject>& obj) {
					return obj->IsMarkedForDestruction();
				}),
			gameObjects.end());
	}
}