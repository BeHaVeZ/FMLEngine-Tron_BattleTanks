#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Timer.h"
#include <iostream>

class MoveCommand : public Command {
public:
    MoveCommand(GameObject* object, glm::vec2 direction, float distance)
        : gameObject(object), direction(direction), moveDistance(distance) {
    }

    void Execute() override {
        if (gameObject) {
            auto transform = gameObject->GetComponent<TransformComponent>();
            if (transform) {
                glm::vec2 newPosition = transform->GetLocalPosition() + direction * moveDistance * Timer::Instance().GetDeltaTime();
                transform->SetPosition(newPosition);
            }
        }
    }

private:
    GameObject* gameObject;
    glm::vec2 direction;
    float moveDistance;
};
