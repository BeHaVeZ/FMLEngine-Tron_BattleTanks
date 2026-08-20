#include "BoxCollider.h"
#include "DebugDraw.h"
#include "TransformComponent.h"
#include "Logger.h"

namespace FML
{
	void BoxCollider::SyncToTransform()
	{
        auto transform = gameObject->GetComponent<TransformComponent>();
        if (transform)
        {
            box.w = static_cast<int>(transform->IsSizeSet() ? transform->GetWidth() : box.w);
            box.h = static_cast<int>(transform->IsSizeSet() ? transform->GetHeight() : box.h);

            box.x = static_cast<int>(transform->GetWorldPosition().x - (transform->GetPivot().x * box.w));
            box.y = static_cast<int>(transform->GetWorldPosition().y - (transform->GetPivot().y * box.h));
        }
	}

	void BoxCollider::Update(float)
	{
		SyncToTransform();
	}
	void BoxCollider::Render(SDL_Renderer*)
	{
	}
}