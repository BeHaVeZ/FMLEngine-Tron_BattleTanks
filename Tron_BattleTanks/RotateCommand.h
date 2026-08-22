#pragma once
#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <iostream>

namespace FML
{
	class RotateCommand : public Command {
	public:
		RotateCommand(GameObject* gameObject, float angle = 0) : gameObject(gameObject), angle(angle) {}

		void Execute() override {
			if (GameObject* object = gameObject.Get()) {
				auto transform = object->GetComponent<TransformComponent>();
				if (transform) {
					transform->SetRotation(angle);
				}
			}
		}

	private:
		GameObjectRef gameObject;
		float angle;
	};
}

