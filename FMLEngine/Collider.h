#pragma once
#include "Component.h"
#include <SDL.h>
#include <memory>
#include <functional>

namespace FML
{
	class CollisionManager;

	class Collider : public Component
	{
	public:
		Collider();
		virtual ~Collider();
		virtual SDL_Rect GetBoundingBox() const = 0;


		std::function<void(Collider* other)> OnCollision;
		std::function<void(Collider* other)> OnTrigger;

		bool isStatic;
		bool isTrigger;
	};
}