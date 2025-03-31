#pragma once
#include "Collider.h"

namespace FML
{
	class BoxCollider : public Collider {
	public:
		BoxCollider(const SDL_Rect& boundingBox) : box(boundingBox) {}

		SDL_Rect GetBoundingBox() const override { return box; }
		void SetBoundingBox(const SDL_Rect& newBox) { box = newBox; }

		void Update(float deltaTime) override;
		void Render(SDL_Renderer* renderer) override;

	private:
		SDL_Rect box;
	};
}

