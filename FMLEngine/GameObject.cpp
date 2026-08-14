#include "GameObject.h"
#include "TransformComponent.h"
#include "Collider.h"
#include "CollisionManager.h"

namespace FML
{
	GameObject::GameObject(const std::string& tag) : tag(tag), parent(nullptr), isMarkedForDestruction(false)
	{
		auto transform = std::make_unique<TransformComponent>();
		AddComponent(std::move(transform));
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

		if (child->parent)
		{
			child->parent->RemoveChild(child.get());
		}

		child->parent = this;

		auto transform = child->GetComponent<TransformComponent>();
		if (transform)
		{
			transform->UpdateWorldPosition();
		}

		children.push_back(std::move(child));
	}


	bool GameObject::RemoveChild(GameObject* child)
	{
		auto it = std::find_if(children.begin(), children.end(),
			[child](const std::unique_ptr<GameObject>& c) { return c.get() == child; });
		if (it != children.end()) {
			(*it)->parent = nullptr;
			children.erase(it);
			return true;
		}
		return false;
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

