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
   // float GetWorldX() const { return worldX; }
   // float GetWorldY() const { return worldY; }
   // float GetLocalX() const { return localX; }
   // float GetLocalY() const { return localY; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

private:
    //float localX, localY;
    glm::vec2 localPosition;
    float rotation; 
    //float worldX, worldY;
    glm::vec2 worldPosition;
    float worldRotation; 
    float width, height;
    bool isDirty;
};
