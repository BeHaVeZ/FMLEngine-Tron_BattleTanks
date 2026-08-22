#include "GameObject.h"
#include "TransformComponent.h"
#include "Collider.h"
#include "CollisionManager.h"
#include "GameObjectDestroyedEvent.h"

namespace FML
{
	GameObjectRef::GameObjectRef(GameObject* object)
		: token(object ? std::weak_ptr<GameObject*>(object->selfToken) : std::weak_ptr<GameObject*>())
	{
	}

	GameObject::GameObject(const std::string& tag)
		: tag(tag), parent(nullptr), isMarkedForDestruction(false), selfToken(std::make_shared<GameObject*>(this))
	{
		auto transform = std::make_unique<TransformComponent>();
		AddComponent(std::move(transform));
	}

	GameObject::~GameObject()
	{
		*selfToken = nullptr;
	}


	void GameObject::AddComponent(std::unique_ptr<Component> component)
	{
		component->SetOwner(this);
		components.push_back(std::move(component));
	}

	void GameObject::AddChild(std::unique_ptr<GameObject> child)
	{
		if (!child or child.get() == this or child->HasParent())
		{
			return;
		}

		for (const GameObject* ancestor = parent; ancestor; ancestor = ancestor->parent)
		{
			if (ancestor == child.get())
			{
				return;
			}
		}

		child->parent = this;

		auto transform = child->GetComponent<TransformComponent>();
		if (transform)
		{
			transform->UpdateWorldPosition();
		}

		children.push_back(std::move(child));
	}

	std::unique_ptr<GameObject> GameObject::DetachChild(GameObject* child)
	{
		auto it = std::find_if(children.begin(), children.end(),
			[child](const std::unique_ptr<GameObject>& c) { return c.get() == child; });
		if (it == children.end())
		{
			return nullptr;
		}

		std::unique_ptr<GameObject> detached = std::move(*it);
		children.erase(it);
		detached->parent = nullptr;

		if (auto* transform = detached->GetComponent<TransformComponent>())
		{
			transform->MarkDirty();
		}

		return detached;
	}

	bool GameObject::RemoveChild(GameObject* child)
	{
		return DetachChild(child) != nullptr;
	}

	void GameObject::CleanupDestroyedChildren()
	{
		for (auto& child : children)
		{
			child->CleanupDestroyedChildren();
		}

		for (auto& child : children)
		{
			if (child->IsMarkedForDestruction())
			{
				child->GetSubject().Notify(GameObjectDestroyedEvent(child.get()));
			}
		}

		children.erase(
			std::remove_if(children.begin(), children.end(),
				[](const std::unique_ptr<GameObject>& child) {
					return child->IsMarkedForDestruction();
				}),
			children.end());
	}

	GameObject* GameObject::FindChildByTag(const std::string& searchTag) const
	{
		for (const auto& child : children) {
			if (child->GetTag() == searchTag) {
				return child.get();
			}
		}
		return nullptr;
	}


	void GameObject::Initialize()
	{
		for (auto& component : components)
		{
			component->Initialize();
		}

		for (auto& child : children)
		{
			child->Initialize();
		}
	}

	void GameObject::Update(float deltaTime)
	{
		if (isMarkedForDestruction) return;

		for (auto& component : components) {
			component->Update(deltaTime);
		}
		for (auto& child : children) {
			child->Update(deltaTime);
		}
	}

	void GameObject::Render(SDL_Renderer* renderer)
	{
		if (isMarkedForDestruction) return;

		for (auto& component : components) {
			component->Render(renderer);
		}
		for (auto& child : children) {
			child->Render(renderer);
		}
	}

	void GameObject::SetTag(const std::string& newTag)
	{
		tag = newTag;
	}

	const std::string& GameObject::GetTag() const
	{
		return tag;
	}

	GameObject* GameObject::GetParent() const
	{
		return parent;
	}

	const std::vector<std::unique_ptr<GameObject>>& GameObject::GetChildren() const
	{
		return children;
	}

	bool GameObject::HasChildren() const
	{
		return !children.empty();
	}

	bool GameObject::HasParent() const
	{
		return parent != nullptr;
	}

	void GameObject::Destroy()
	{
		isMarkedForDestruction = true;
	}
}
