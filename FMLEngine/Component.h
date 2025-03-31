#pragma once
#include "SDL.h"

namespace FML
{
	class GameObject;

	class Component {
	public:
		virtual ~Component() = default;

		virtual void Initialize() {};
		virtual void Update(float) {};
		virtual void Render(SDL_Renderer*) {};

		void SetOwner(GameObject* owner) 
		{
			this->gameObject = owner;
		}

		GameObject* GetOwner() { return gameObject; }

	protected:
		GameObject* gameObject = nullptr;
	};
}