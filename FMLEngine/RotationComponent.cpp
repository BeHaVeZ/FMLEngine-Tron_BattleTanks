#include "RotationComponent.h"
#include <SDL.h> 

RotationComponent::RotationComponent(float radius, float speed, float centerX, float centerY)
	: radius(radius), speed(speed* (float)M_PI / 180.0f),
	centerX(centerX), centerY(centerY), angle(0.0f) {
}

void RotationComponent::Initialize()
{
}

void RotationComponent::Update(float deltaTime) {
	angle += speed * deltaTime;

	float newX = centerX + radius * cos(angle);
	float newY = centerY + radius * sin(angle);

	if (gameObject && gameObject->GetComponent<TransformComponent>()) {
		gameObject->GetComponent<TransformComponent>()->SetPosition(newX, newY);
	}
}
