#include "PrefabRegistry.h"
#include "TextureComponent.h"
#include "SceneManager.h"
#include "HealthComponent.h"
#include "HealthUIComponent.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "../Tron_BattleTanks/ShootComponent.h"
#include "../Tron_BattleTanks/BulletMoveComponent.h"
#include "../Tron_BattleTanks/BulletCollisionBehaviorComponent.h"

namespace FML
{

	PrefabRegistry& PrefabRegistry::Instance()
	{
		static PrefabRegistry instance;
		return instance;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateRedTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/RedTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->AddComponent(std::move(tankTexture));
		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		auto tankHealth = std::make_unique<HealthComponent>(100);
		tank->GetSubject().AddObserver(tankHealth.get());
		tank->AddComponent(std::move(tankHealth));

		auto turret = std::make_unique<GameObject>("Turret");
		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Blue_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->GetComponent<TransformComponent>()->CentralizePivotOnTexture(turretTexture.get());
		turret->AddComponent(std::move(turretTexture));
		//turret->GetComponent<TransformComponent>()->SetPosition({ 0, -6 });
		//turret->GetComponent<TransformComponent>()->OffsetPivotPoint({ 0,9 });

		float shootAllowanceRange = 30.f;

		auto shootComponent = std::make_unique<ShootComponent>(turret.get(), shootAllowanceRange, 0.f);
		turret->AddComponent(std::move(shootComponent));

		tank->AddChild(std::move(turret));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateBlueTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/BlueTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->AddComponent(std::move(tankTexture));
		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		//playerCollider->isStatic = true;
		tank->AddComponent(std::move(playerCollider));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateYellowTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/YellowTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->AddComponent(std::move(tankTexture));
		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		auto turret = std::make_unique<GameObject>("Turret");
		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Yellow_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->GetComponent<TransformComponent>()->CentralizePivotOnTexture(turretTexture.get());
		turret->AddComponent(std::move(turretTexture));
		turret->GetComponent<TransformComponent>()->SetPosition({ 0, -6 });
		//turret->GetComponent<TransformComponent>()->OffsetPivotPoint({ 0,9 });

		tank->AddChild(std::move(turret));
		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateBulletPrefab(glm::vec2 spawnPosition, glm::vec2 moveDirection, const std::string tag) const
	{
		auto bullet = std::make_unique<GameObject>(tag);

		auto bulletTexture = std::make_unique<TextureComponent>("data/artassets/Bullet.png", SceneManager::Instance().GetRenderer());

		auto bulletTransform = bullet->GetComponent<TransformComponent>();

		bulletTransform->CentralizePivotOnTexture(bulletTexture.get());
		bullet->AddComponent(std::move(bulletTexture));

		bullet->GetComponent<TransformComponent>()->SetPosition(spawnPosition - bulletTransform->GetPivot());

		auto bulletMoveComponent = std::make_unique<BulletMoveComponent>(moveDirection, 100.f);
		bullet->AddComponent(std::move(bulletMoveComponent));

		auto bulletBehavior = std::make_unique<BulletCollisionBehaviorComponent>();
		bullet->AddComponent(std::move(bulletBehavior));

		auto bulletCollider = std::make_unique<BoxCollider>(SDL_Rect{0, 0,bullet->GetComponent<TextureComponent>()->GetDefaultWidth(),bullet->GetComponent<TextureComponent>()->GetDefaultHeight()});

		GameObject* bulletRaw = bullet.get();
		bulletCollider->OnCollision = [bulletRaw](Collider* other)
			{
				auto behavior = bulletRaw->GetComponent<BulletCollisionBehaviorComponent>();
				if (behavior)
					behavior->OnCollision(bulletRaw, other);
			};

		bullet->AddComponent(std::move(bulletCollider));

		return bullet;
	}
}

