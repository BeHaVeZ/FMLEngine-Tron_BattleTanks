#include "TransformComponent.h"
#include "TextureComponent.h"
#include <cmath>

namespace FML
{

	TransformComponent::TransformComponent(glm::vec2 position, float rotation, glm::vec2 pivot)
		: localPosition(position), localRotation(rotation), pivot(pivot), width(0), height(0), worldPosition(position), worldRotation(rotation), isDirty(true)
	{
	}

	void TransformComponent::SetPosition(glm::vec2 newPosition)
	{
		if (newPosition != localPosition)
		{
			localPosition = newPosition;
			MarkDirty();
		}
	}

	void TransformComponent::SetSize(float newWidth, float newHeight)
	{
		width = newWidth;
		height = newHeight;
	}

	void TransformComponent::CentralizePivotOnTexture(TextureComponent* texture)
	{
		if (texture)
		{
			SetPivot({ texture->GetDefaultWidth() / 2, texture->GetDefaultHeight() / 2 });
		}
	}

	void TransformComponent::SetRotation(float newRotation)
	{
		if (newRotation != localRotation) {
			localRotation = newRotation;
			MarkDirty();
		}
	}

	void TransformComponent::SetPivot(glm::vec2 newPivot)
	{
		if (newPivot != pivot)
		{
			pivot = newPivot;
			MarkDirty();
		}
	}

	void TransformComponent::OffsetPivotPoint(glm::vec2 offset)
	{
		SetPivot(pivot + offset);
	}

	void TransformComponent::Update(float)
	{
		if (isDirty)
		{
			UpdateWorldPosition();
			isDirty = false;
		}
	}

	void TransformComponent::UpdateWorldPosition()
	{
		if (gameObject->HasParent()) {
			auto parentTransform = gameObject->GetParent()->GetComponent<TransformComponent>();
			if (parentTransform)
			{
				glm::vec2 pivotOffset = glm::vec2(width * pivot.x, height * pivot.y);
				glm::vec2 positionOffset = localPosition - pivotOffset;

				float rotationRadians = parentTransform->localRotation * (float)M_PI / 180;
				glm::vec2 rotatedPositionOffset = glm::vec2
				(
					positionOffset.x * cos(rotationRadians) - positionOffset.y * sin(rotationRadians),
					positionOffset.x * sin(rotationRadians) + positionOffset.y * cos(rotationRadians)
				);

				worldPosition = parentTransform->worldPosition + rotatedPositionOffset + pivotOffset;
				worldRotation = parentTransform->worldRotation + localRotation;
			}
		}
		else
		{
			worldPosition = localPosition;
			worldRotation = localRotation;
		}
	}

	void TransformComponent::MarkDirty()
	{
		isDirty = true;
		for (auto& child : gameObject->GetChildren())
		{
			auto childTransform = child->GetComponent<TransformComponent>();
			if (childTransform)
			{
				childTransform->MarkDirty();
			}
		}
	}

	bool TransformComponent::IsSizeSet() const
	{
		return width > 0.0f && height > 0.0f;
	}


}

