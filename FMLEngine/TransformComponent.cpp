#include "TransformComponent.h"

TransformComponent::TransformComponent(float x, float y)
    : localX(x), localY(y), width(0), height(0), worldX(x), worldY(y), isDirty(true) {
}

void TransformComponent::SetPosition(float newX, float newY) {
    if (newX != localX || newY != localY) {
        localX = newX;
        localY = newY;
        MarkDirty();
    }
}

void TransformComponent::SetSize(float newWidth, float newHeight) {
    width = newWidth;
    height = newHeight;
}

void TransformComponent::Update() {
    if (isDirty) {
        UpdateWorldPosition();
        isDirty = false;
    }
}

void TransformComponent::UpdateWorldPosition() 
{
    if (gameObject and gameObject->GetParent()) 
    {
        auto parentTransform = gameObject->GetParent()->GetComponent<TransformComponent>();
        if (parentTransform) 
        {
            worldX = parentTransform->worldX + localX;
            worldY = parentTransform->worldY + localY;
        }
    }
    else 
    {
        worldX = localX;
        worldY = localY;
    }

    if (gameObject) 
    {
        for (auto& child : gameObject->GetChildren()) 
        {
            auto childTransform = child->GetComponent<TransformComponent>();
            if (childTransform) 
            {
                childTransform->MarkDirty();
            }
        }
    }
}

void TransformComponent::MarkDirty() {
    isDirty = true;
    for (auto& child : gameObject->GetChildren()) {
        auto childTransform = child->GetComponent<TransformComponent>();
        if (childTransform) {
            childTransform->MarkDirty();
        }
    }
}

bool TransformComponent::IsSizeSet() const {
    return width > 0.0f and height > 0.0f;
}