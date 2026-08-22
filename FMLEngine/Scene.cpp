#include "Scene.h"
#include "TransformComponent.h"
#include "CollisionManager.h"
#include "InputHandler.h"
#include "GameObjectDestroyedEvent.h"

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

	void Scene::Cleanup()
	{
		gameObjects.clear();
	}

	void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject)
	{
		if (gameObject)
		{
			gameObjects.emplace_back(std::move(gameObject));
		}
	}

	void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject, const glm::vec2& position)
	{
		if (gameObject)
		{
			if (auto* transform = gameObject->GetComponent<TransformComponent>())
			{
				transform->SetPosition(position);
			}
			gameObjects.emplace_back(std::move(gameObject));
		}
	}

	void Scene::UpdateGameObjects(float deltaTime)
	{
		const size_t count = gameObjects.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (!gameObjects[i]->IsMarkedForDestruction())
			{
				gameObjects[i]->Update(deltaTime);
			}
		}
	}

	void Scene::RenderGameObjects(SDL_Renderer* renderer)
	{
		for (auto& gameObject : gameObjects)
		{
			if (!gameObject->IsMarkedForDestruction())
			{
				gameObject->Render(renderer);
			}
		}
	}

	GameObject* Scene::FindGameObjectByTag(std::string_view tag)
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

	std::vector<GameObject*> Scene::FindGameObjectsByTag(std::string_view tag) const
	{
		std::vector<GameObject*> found;
		FindGameObjectsByTag(tag, found);
		return found;
	}

	void Scene::FindGameObjectsByTag(std::string_view tag, std::vector<GameObject*>& outFound) const
	{
		outFound.clear();
		for (const auto& gameObject : gameObjects)
		{
			if (gameObject->GetTag() == tag && !gameObject->IsMarkedForDestruction())
			{
				outFound.push_back(gameObject.get());
			}
		}
	}

	void Scene::CleanupDestroyedGameObjects()
	{
		const size_t count = gameObjects.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (gameObjects[i]->IsMarkedForDestruction())
			{
				gameObjects[i]->GetSubject().Notify(GameObjectDestroyedEvent(gameObjects[i].get()));
			}
			else
			{
				gameObjects[i]->CleanupDestroyedChildren();
			}
		}

		gameObjects.erase(
			std::remove_if(gameObjects.begin(), gameObjects.end(),
				[](const std::unique_ptr<GameObject>& obj) {
					return obj->IsMarkedForDestruction();
				}),
			gameObjects.end());
	}
	void Scene::OnEnter()
	{
	}

	void Scene::OnExit()
	{
	}
}