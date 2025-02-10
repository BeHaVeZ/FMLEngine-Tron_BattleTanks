#include "GameObject.h"
#include "TransformComponent.h"

GameObject::GameObject(const std::string& tag) : tag(tag) {
    auto transform = std::make_unique<TransformComponent>();
    AddComponent(std::move(transform));
}

void GameObject::AddComponent(std::unique_ptr<Component> component) {
    component->SetOwner(this);
    components.push_back(std::move(component));
}

void GameObject::Update(float deltaTime) {
    for (auto& component : components) {
        component->Update(deltaTime);
    }
}

void GameObject::Render(SDL_Renderer* renderer) {
    for (auto& component : components) {
        component->Render(renderer);
    }
}
