#include "BoxCollider.h"
#include "DebugDraw.h"
#include "TransformComponent.h"
#include "Logger.h"

namespace FML
{
	void BoxCollider::Update(float)
	{
		auto transform = gameObject->GetComponent<TransformComponent>();
		if (transform) 
		{
			box.x = static_cast<int>(transform->GetWorldPosition().x);
			box.y = static_cast<int>(transform->GetWorldPosition().y);
		}
	}
	void BoxCollider::Render(SDL_Renderer*)
	{
		DebugDraw::DrawRectangle({ box.x,box.y }, { box.w,box.h }, { 1,0,1,.1f });
	}
}