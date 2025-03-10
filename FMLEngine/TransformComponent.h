#pragma once
#include "Component.h"
#include "GameObject.h"
#include <glm.hpp>


class TransformComponent final : public Component {
public:
	TransformComponent(glm::vec2 position = {}, float rotation = 0.0f);
	void SetPosition(glm::vec2 newPosition);
	void SetRotation(float newRotation);
	void SetSize(float newWidth, float newHeight);

	void Update(float deltaTime) override;
	void UpdateWorldPosition();
	void MarkDirty();

	bool IsSizeSet() const;

	glm::vec2  GetWorldPosition() const { return worldPosition; }
	glm::vec2 GetLocalPosition() const { return localPosition; }
	float GetWorldRotation() const { return worldRotation; }
	float GetLocalRotation() const { return localRotation; }
	float GetWidth() const { return width; }
	float GetHeight() const { return height; }

private:
	glm::vec2 localPosition, worldPosition;
	float localRotation, worldRotation;
	float width, height;
	bool isDirty;
};
