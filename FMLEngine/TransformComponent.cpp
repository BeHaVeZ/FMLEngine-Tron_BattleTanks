#include "TransformComponent.h"
//#include <cmath>

TransformComponent::TransformComponent(float x, float y, float rotation)
    : localX(x), localY(y), rotation(rotation), width(0), height(0), worldX(x), worldY(y), worldRotation(rotation), isDirty(true) {
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

void TransformComponent::SetRotation(float newRotation) {
    if (newRotation != rotation) {
        rotation = newRotation;
        MarkDirty();
    }
}

float TransformComponent::GetRotation() const {
    return rotation;
}

void TransformComponent::Update(float) {
    if (isDirty) {
        UpdateWorldPosition();
        isDirty = false;
    }
}

void TransformComponent::UpdateWorldPosition() {
    if (gameObject->HasParent()) {
        auto parentTransform = gameObject->GetParent()->GetComponent<TransformComponent>();
        if (parentTransform) {
            worldX = parentTransform->worldX + localX * cosf(parentTransform->rotation * (float)M_PI / 180) - localY * sinf(parentTransform->rotation * (float)M_PI / 180);
            worldY = parentTransform->worldY + localX * sinf(parentTransform->rotation * (float)M_PI / 180) + localY * cosf(parentTransform->rotation * (float)M_PI / 180);
            worldRotation = parentTransform->worldRotation + rotation;
        }
    }
    else {
        worldX = localX;
        worldY = localY;
        worldRotation = rotation;
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
    return width > 0.0f && height > 0.0f;
}
