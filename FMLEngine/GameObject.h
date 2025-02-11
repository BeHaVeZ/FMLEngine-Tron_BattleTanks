#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Component.h"
#include "SDL.h"

class GameObject final{
public:
    GameObject(const std::string& tag = "");

    template <typename T>
    T* GetComponent() {
        for (auto& component : components) {
            if (auto specificComponent = dynamic_cast<T*>(component.get())) {
                return specificComponent;
            }
        }
        return nullptr;
    }

    void AddComponent(std::unique_ptr<Component> component);
    void Update(float deltaTime);
    void Render(SDL_Renderer* renderer);


    void SetTag(const std::string& newTag) { tag = newTag; }
    const std::string& GetTag() const { return tag; }

private:
    std::string tag;
    std::vector<std::unique_ptr<Component>> components;
};
