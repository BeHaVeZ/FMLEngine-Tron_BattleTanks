#pragma once
#include "Observer.h"
#include "Component.h"
#include "BulletHitEvent.h"
#include "PrefabRegistry.h"
#include "SceneManager.h"
#include "Logger.h"
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

                Logger::Log(LogLevel::Debug, "Bullet hit object with tag %s", target ? target->GetTag().c_str() : "None");
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

                SoundHelper::PlayRandomSound({ 10,13 }, .3f);

                auto poof = PrefabRegistry::Instance().CreateBulletExplosionPrefab(position);
                SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(poof));

                Logger::Log(LogLevel::Debug, "Bullet destroyed. Playing exit effect at position %.1f, %.1f", position.x, position.y);
            }
        }
    };
}
