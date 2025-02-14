#include "RotatorComponent.h"
#include <SDL.h>

RotatorComponent::RotatorComponent(float radius, float speed, float centerX, float centerY)
    : radius(radius), speed(speed* M_PI / 180.0f),
    centerX(centerX), centerY(centerY), angle(0.0f) {
}

void RotatorComponent::Initialize() {
    
}

void RotatorComponent::Update(float deltaTime) {
    angle += speed * deltaTime;

    float newX = centerX + radius * cos(angle);
    float newY = centerY + radius * sin(angle);

    if (gameObject && gameObject->GetComponent<TransformComponent>()) {
        gameObject->GetComponent<TransformComponent>()->SetPosition(newX, newY);
    }
}
