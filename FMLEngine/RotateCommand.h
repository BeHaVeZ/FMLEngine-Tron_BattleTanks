#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <iostream>

class RotateCommand : public Command {
public:
    RotateCommand(GameObject* gameObject, float angle = 0) : gameObject(gameObject), angle(angle) {}

    void Execute() override {
        if (gameObject) {
            auto transform = gameObject->GetComponent<TransformComponent>();
            if (transform) {
                transform->SetRotation(angle);
            }
        }
    }

private:
    GameObject* gameObject;
    float angle;
};
