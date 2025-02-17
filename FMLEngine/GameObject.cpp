#include "GameObject.h"
#include "TransformComponent.h"

GameObject::GameObject(const std::string& tag) : tag(tag), parent(nullptr) 
{
	auto transform = std::make_unique<TransformComponent>();
	AddComponent(std::move(transform));
}


void GameObject::AddComponent(std::unique_ptr<Component> component) 
{
	component->SetOwner(this);
	components.push_back(std::move(component));
}

bool GameObject::RemoveComponent(const Component* component) 
{
	auto it = std::find_if(components.begin(), components.end(),
		[component](const std::unique_ptr<Component>& c) { return c.get() == component; });
	if (it != components.end()) {
		components.erase(it);
		return true;
	}
	return false;
}

bool GameObject::HasComponent() const 
{
	return !components.empty();
}

void GameObject::AddChild(std::unique_ptr<GameObject> child)
{
	if (!child || child.get() == this || child->HasParent())
	{
		return;
	}

	auto transform = child->GetComponent<TransformComponent>();
	if (transform)
	{
		transform->UpdateWorldPosition();
	}

	if (child->parent)
	{
		child->parent->RemoveChild(child.get());
	}

	child->parent = this;

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


void GameObject::Reparent(GameObject* newParent) 
{
	if (parent) {
		parent->RemoveChild(this);
	}
	newParent->AddChild(std::unique_ptr<GameObject>(this));
	parent = newParent;
}

void GameObject::Update(float deltaTime)
{
	for (auto& component : components) {
		component->Update(deltaTime);
	}
	for (auto& child : children) {
		child->Update(deltaTime);
	}
}

void GameObject::Render(SDL_Renderer* renderer)
{
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

void GameObject::SetParent(GameObject* newParent)
{
	if (newParent != nullptr and parent == nullptr)
	{
		parent = newParent;
	}
}

void GameObject::Unparent()
{
	if (parent != nullptr)
	{
		parent = nullptr;
	}
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
