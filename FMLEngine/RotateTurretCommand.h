#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"

class RotateTurretCommand : public Command {
public:
    RotateTurretCommand(GameObject* gameObject, float rotationSpeed = .5f)
        : gameObject(gameObject), rotationSpeed(rotationSpeed) 
    {
    }

    void Execute() override 
    {
        if (gameObject) {
            auto transform = gameObject->GetComponent<TransformComponent>();
            if (transform) {
                float currentRotation = transform->GetLocalRotation();

                currentRotation += rotationSpeed;

                transform->SetRotation(currentRotation);
            }
        }
    }

private:
    GameObject* gameObject;
    float rotationSpeed;
};
