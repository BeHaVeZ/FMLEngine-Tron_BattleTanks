#pragma once
#include "Component.h"
#include "GameObject.h"

class TransformComponent final : public Component {
public:
    TransformComponent(float x = 0.0f, float y = 0.0f, float rotation = 0.0f);
    void SetPosition(float newX, float newY);
    void SetSize(float newWidth, float newHeight);
    void SetRotation(float newRotation);
    float GetRotation() const;

    void Update(float deltaTime) override;
    void UpdateWorldPosition();
    void MarkDirty();

    bool IsSizeSet() const;

    float GetWorldX() const { return worldX; }
    float GetWorldY() const { return worldY; }
    float GetLocalX() const { return localX; }
    float GetLocalY() const { return localY; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

private:
    float localX, localY;
    float rotation; 
    float worldX, worldY;
    float worldRotation; 
    float width, height;
    bool isDirty;
};
