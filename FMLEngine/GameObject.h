#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include "Component.h"
#include "SDL.h"

class GameObject final {
public:
    GameObject(const std::string& tag = "");
    ~GameObject() = default;

    template <typename T>
    T* GetComponent() const {
        for (auto& component : components) {
            if (auto* casted = dynamic_cast<T*>(component.get())) {
                return casted;
            }
        }
        return nullptr;
    }
    void AddComponent(std::unique_ptr<Component> component);
    bool RemoveComponent(const Component* component);
    bool HasComponent() const;

    void AddChild(std::unique_ptr<GameObject> child);
    bool RemoveChild(GameObject* child);
    GameObject* FindChildByTag(const std::string& tag) const;
    void Reparent(GameObject* newParent);

    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);

    void SetTag(const std::string& newTag);
    const std::string& GetTag() const;
	void Unparent();
    GameObject* GetParent() const;
    const std::vector<std::unique_ptr<GameObject>>& GetChildren() const;
    bool HasChildren() const;
    bool HasParent() const;

private:
    std::string tag;
    std::vector<std::unique_ptr<Component>> components;
    std::vector<std::unique_ptr<GameObject>> children;
    GameObject* parent = nullptr;
};
