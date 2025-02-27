#pragma once
#include "Component.h"
#include "GameObject.h"
#include <glm.hpp>


class TransformComponent final : public Component {
public:
    TransformComponent(glm::vec2 position = {}, float rotation = 0.0f);
    void SetPosition(glm::vec2 newPosition);
    void SetSize(float newWidth, float newHeight);
    void SetRotation(float newRotation);
    float GetRotation() const;

    void Update(float deltaTime) override;
    void UpdateWorldPosition();
    void MarkDirty();

    bool IsSizeSet() const;

    glm::vec2  GetWorldPosition() const { return worldPosition; }
    glm::vec2 GetLocalPosition() const { return localPosition; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

private:
    glm::vec2 localPosition;
    float rotation; 
    glm::vec2 worldPosition;
    float worldRotation; 
    float width, height;
    bool isDirty;
};
