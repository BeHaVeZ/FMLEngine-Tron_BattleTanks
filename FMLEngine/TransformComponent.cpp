#include "TransformComponent.h"

TransformComponent::TransformComponent(glm::vec2 position, float rotation)
    : localPosition(position), rotation(rotation), width(0), height(0), worldPosition(position), worldRotation(rotation), isDirty(true) {
}

void TransformComponent::SetPosition(glm::vec2 newPosition) {
    if (newPosition != localPosition) {
        localPosition = newPosition;
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
            worldPosition.x = parentTransform->worldPosition.x + localPosition.x * cosf(parentTransform->rotation * (float)M_PI / 180) - localPosition.y * sinf(parentTransform->rotation * (float)M_PI / 180);
            worldPosition.y = parentTransform->worldPosition.y + localPosition.x * sinf(parentTransform->rotation * (float)M_PI / 180) + localPosition.y * cosf(parentTransform->rotation * (float)M_PI / 180);
            worldRotation = parentTransform->worldRotation + rotation;
        }
    }
    else {
        worldPosition = localPosition;
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
