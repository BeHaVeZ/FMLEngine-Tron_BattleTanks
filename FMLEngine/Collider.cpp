#include "Collider.h"
#include "CollisionManager.h"

namespace FML
{
	Collider::Collider() : isStatic(false), isTrigger(false)
	{
		CollisionManager::Instance().RegisterCollider(this);
	}

	Collider::~Collider()
	{
		CollisionManager::Instance().UnregisterCollider(this);
	}
}
