#pragma once
#include "Component.h"
#include "DamageEvent.h"
#include "GameObject.h"
#include "HealthComponent.h"
#include "Observer.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "ScreenShake.h"
#include "TeleportManager.h"
#include "TransformComponent.h"

namespace FML
{
	class PlayerRespawnComponent final : public Component, public Observer
	{
	public:
		void HandleEvent(const Event& event) override
		{
			if (!dynamic_cast<const DamageEvent*>(&event) || !gameObject)
			{
				return;
			}

			auto* health = gameObject->GetComponent<HealthComponent>();
			auto* transform = gameObject->GetComponent<TransformComponent>();
			auto* scene = SceneManager::Instance().GetCurrentScene();
			if (!health || health->GetCurrentHealth() <= 0 || !transform || !scene)
			{
				return;
			}

			ScreenShake::Instance().TriggerPlayerHit();

			scene->AddGameObject(PrefabRegistry::Instance().CreateTankExplosionPrefab(transform->GetWorldPosition()));
			transform->SetPosition(TeleportManager::Instance().GetRandomTeleportPosition());
			transform->UpdateWorldPosition();
			scene->AddGameObject(PrefabRegistry::Instance().CreateTpEffect(transform->GetWorldPosition()));
		}
	};
}
