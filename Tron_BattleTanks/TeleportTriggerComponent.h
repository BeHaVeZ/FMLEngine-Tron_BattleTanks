#pragma once
#include "Component.h"
#include "Collider.h"

namespace FML
{
    class TeleportTriggerComponent : public Component
    {
    public:
        void Update(float) override {}
        void Render(SDL_Renderer*) override {}

        void Initialize(Collider* collider)
        {
            if (!collider)
                return;

            collider->OnTrigger = [this](Collider* other)
                {
                    if (!other) return;

                    const std::string& tag = other->GetOwner()->GetTag();
                    if (tag == "Player1" || tag == "Player2")
                    {
                        glm::vec2 randomPosition = TeleportManager::Instance().GetRandomTeleportPosition();

                        auto* transform = other->GetOwner()->GetComponent<TransformComponent>();
                        if (transform)
                        {
                            transform->SetPosition(randomPosition);
							ServiceLocator::GetSoundSystem().PlaySound(SoundId::PlayerExplosion1, ServiceLocator::GetSoundSystem().GetCurrentVolume() + .3f);

                            transform->UpdateWorldPosition();
                            auto tpEffect = PrefabRegistry::Instance().CreateTpEffect(transform->GetWorldPosition());
                            SceneManager::Instance().GetCurrentScene()->AddGameObject(std::move(tpEffect));
                        }
                    }
                };
        };
    };
}
