#include "TransformComponent.h"
#include "TextureComponent.h"
#include "DebugDraw.h"
#include <cmath>

namespace FML
{

	TransformComponent::TransformComponent(glm::vec2 position, float rotation, glm::vec2 pivot)
		: localPosition(position), localRotation(rotation), pivot(pivot),
		width(0), height(0), isDirty(true), isMoving(false),
		localMatrix(1.0f), worldMatrix(1.0f)
	{
	}

	void TransformComponent::SetPosition(glm::vec2 newPosition)
	{
		if (newPosition != localPosition)
		{
			localPosition = newPosition;
			MarkDirty();
			MarkMoving(true);
		}
	}

	void TransformComponent::SetRotation(float newRotation)
	{
		if (newRotation != localRotation) 
		{
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

	void TransformComponent::SetSize(float newWidth, float newHeight)
	{
		width = newWidth;
		height = newHeight;
	}

	void TransformComponent::CentralizePivotOnTexture(TextureComponent* texture)
	{
		if (texture)
		{
			SetSize((float)texture->GetDefaultWidth(), (float)texture->GetDefaultHeight());
			SetPivot({ 0.5f, 0.5f });
		}
	}

	void FML::TransformComponent::Render(SDL_Renderer*)
	{
		glm::vec2 pos = GetWorldPosition();

		glm::vec2 right = { worldMatrix[0][0], worldMatrix[0][1] };
		glm::vec2 up = { -worldMatrix[1][0], -worldMatrix[1][1] };

		float lineLength = 30.0f;

		glm::vec2 endRight = pos + right * lineLength;
		glm::vec2 endUp = pos + up * lineLength;

		DebugDraw::DrawLine(pos, endRight, { 1.0f, 0.0f, 0.0f, 1.0f }); 

		DebugDraw::DrawLine(pos, endUp, { 0.0f, 1.0f, 0.0f, 1.0f }); 

		DebugDraw::DrawCircle(pos, 4.0f, { 0.0f, 0.0f, 1.0f, 1.0f }); 
	}

	void TransformComponent::Update(float)
	{
		if (isDirty)
		{
			UpdateWorldPosition();
			isDirty = false;
		}
		if (isMoving)
		{
			MarkMoving(false);
		}
	}

	void TransformComponent::UpdateWorldPosition()
	{
		glm::mat3 P(1.0f); 
		glm::mat3 R(1.0f); 
		glm::mat3 T(1.0f);

		P[2] = glm::vec3(-pivot, 1.0f);

		float radians = glm::radians(localRotation);
		R[0][0] = cos(radians); R[0][1] = -sin(radians);
		R[1][0] = sin(radians); R[1][1] = cos(radians);

		T[2] = glm::vec3(localPosition, 1.0f);

		localMatrix = T * R * P;

		if (gameObject->HasParent())
		{
			auto* parentTransform = gameObject->GetParent()->GetComponent<TransformComponent>();
			if (parentTransform)
			{
				worldMatrix = parentTransform->worldMatrix * localMatrix;
			}
		}
		else
		{
			worldMatrix = localMatrix;
		}
	}

	glm::vec2 TransformComponent::GetWorldPosition() const
	{
		return glm::vec2(worldMatrix[2][0], worldMatrix[2][1]);
	}

	float TransformComponent::GetWorldRotation() const
	{
		float radians = atan2(worldMatrix[0][1], worldMatrix[0][0]);
		return glm::degrees(radians);
	}

	void TransformComponent::MarkDirty()
	{
		isDirty = true;
		for (auto& child : gameObject->GetChildren())
		{
			auto* childTransform = child->GetComponent<TransformComponent>();
			if (childTransform)
			{
				childTransform->MarkDirty();
			}
		}
	}

	void TransformComponent::MarkMoving(bool moving)
	{
		isMoving = moving;
	}

	bool TransformComponent::IsSizeSet() const
	{
		return width > 0.0f && height > 0.0f;
	}

}
