#pragma once
#include "Component.h"
#include "GameObject.h"
#include <glm.hpp>

namespace FML
{

	class TextureComponent;


	class TransformComponent final : public Component {
	public:
		TransformComponent(glm::vec2 position = {}, float rotation = 0.0f, glm::vec2 pivot = { .5f,.5f });
		void SetPosition(glm::vec2 newPosition);
		void SetRotation(float newRotation);
		void SetPivot(glm::vec2 newPivot);
		void OffsetPivotPoint(glm::vec2 offset);
		void SetSize(float newWidth, float newHeight);

		void CentralizePivotOnTexture(TextureComponent* texture);

		void Update(float deltaTime) override;
		void UpdateWorldPosition();
		void MarkDirty();
		void MarkMoving(bool moving);

		bool IsMoving() const { return isMoving; }

		bool IsSizeSet() const;

		glm::vec2  GetWorldPosition() const { return worldPosition; }
		glm::vec2 GetLocalPosition() const { return localPosition; }
		glm::vec2 GetPivot() const { return pivot; }
		float GetWorldRotation() const { return worldRotation; }
		float GetLocalRotation() const { return localRotation; }
		float GetWidth() const { return width; }
		float GetHeight() const { return height; }

	private:
		glm::vec2 localPosition, worldPosition, pivot;
		float localRotation, worldRotation;
		float width, height;
		bool isDirty,isMoving;
	};


}

