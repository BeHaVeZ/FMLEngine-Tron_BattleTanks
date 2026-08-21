#pragma once
#include "Observer.h"
#include "Component.h"
#include "BulletHitEvent.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "SoundHelper.h"

namespace FML
{
    class BulletObserver : public Component, public Observer
    {
    public:
        void HandleEvent(const Event& event) override
        {
            if (auto* hit = dynamic_cast<const BulletHitEvent*>(&event))
            {
                GameObject* target = hit->GetTarget();
                glm::vec2 position = hit->GetHitPosition();

                if (target && target->GetTag() == "Wall")
                {
                    auto sparks = PrefabRegistry::Instance().CreateHitExplosionPrefab(position);
                    SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(sparks));
                }
            }
            else if (auto* destroyEvent = dynamic_cast<const GameObjectDestroyedEvent*>(&event))
            {
                auto go = destroyEvent->GetDestroyedObject();
                if (!go) return;

                glm::vec2 position = go->GetComponent<TransformComponent>()->GetWorldPosition();

				SoundHelper::PlayRandomSound({ SoundId::Explosion1, SoundId::Explosion4 }, .3f);

                auto poof = PrefabRegistry::Instance().CreateBulletExplosionPrefab(position);
                SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(poof));
            }
        }
    };
}
