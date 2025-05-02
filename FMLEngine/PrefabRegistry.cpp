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
#include "../Tron_BattleTanks/EnemyMovementComponent.h"
#include "../Tron_BattleTanks/ScoreComponent.h"
#include "../Tron_BattleTanks/ScoreUIComponent.h"
#include "../Tron_BattleTanks/EnemyShootComponent.h"
#include "../Tron_BattleTanks/EnemyManagerComponent.h"
#include "../Tron_BattleTanks/TankObserver.h"
#include "../Tron_BattleTanks/GameAdmin.h"
#include "../Tron_BattleTanks/HighscoreManager.h"
#include "../Tron_BattleTanks/TeleportManager.h"
#include "../Tron_BattleTanks/TeleportTriggerComponent.h"

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
		GameAdmin::Instance().RegisterPlayer(tank.get());

		auto observer = std::make_unique<TankObserver>();
		tank->GetSubject().AddObserver(observer.get());
		tank->AddComponent(std::move(observer));

		auto playerHealth = std::make_unique<HealthComponent>(3);
		tank->GetSubject().AddObserver(playerHealth.get());
		tank->AddComponent(std::move(playerHealth));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->GetSubject().AddObserver(playerScore.get());
		tank->AddComponent(std::move(playerScore));

		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/GreenTank.png", SceneManager::Instance().GetRenderer());
		tank->AddComponent(std::move(tankTexture));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tank->GetComponent<TextureComponent>());

		auto turret = std::make_unique<GameObject>("Turret");

		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Blue_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->AddComponent(std::move(turretTexture));

		auto turretTransform = turret->GetComponent<TransformComponent>();
		turretTransform->SetPosition({ 0, 3 });
		turretTransform->OffsetPivotPoint({ 0, .2f });

		float shootAllowanceRange = 28.f;

		auto shootComponent = std::make_unique<ShootComponent>(turret.get(), shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		tank->AddChild(std::move(turret));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateBlueTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto observer = std::make_unique<TankObserver>();
		tank->GetSubject().AddObserver(observer.get());
		tank->AddComponent(std::move(observer));

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/BlueTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y });
		tank->AddComponent(std::move(tankTexture));

		auto tankHealth = std::make_unique<HealthComponent>(1);
		tank->GetSubject().AddObserver(tankHealth.get());
		tank->AddComponent(std::move(tankHealth));

		auto enemyMovement = std::make_unique<EnemyMovementComponent>(75.f);
		tank->AddComponent(std::move(enemyMovement));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth() - 2,tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		//playerCollider->isStatic = true;
		tank->AddComponent(std::move(playerCollider));

		auto shootComponent = std::make_unique<EnemyShootComponent>();
		tank->AddComponent(std::move(shootComponent));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreatePinkTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/PinkTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y});
		tank->AddComponent(std::move(tankTexture));

		auto tankHealth = std::make_unique<HealthComponent>(1);
		tank->GetSubject().AddObserver(tankHealth.get());
		tank->AddComponent(std::move(tankHealth));

		auto enemyMovement = std::make_unique<EnemyMovementComponent>(100.f);
		tank->AddComponent(std::move(enemyMovement));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth() - 2,tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		//playerCollider->isStatic = true;
		tank->AddComponent(std::move(playerCollider));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateGreenTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto observer = std::make_unique<TankObserver>();
		tank->GetSubject().AddObserver(observer.get());
		tank->AddComponent(std::move(observer));

		auto playerHealth = std::make_unique<HealthComponent>(3);
		tank->GetSubject().AddObserver(playerHealth.get());
		tank->AddComponent(std::move(playerHealth));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->GetSubject().AddObserver(playerScore.get());
		tank->AddComponent(std::move(playerScore));

		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/GreenTank.png", SceneManager::Instance().GetRenderer());
		tank->AddComponent(std::move(tankTexture));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tank->GetComponent<TextureComponent>());

		auto turret = std::make_unique<GameObject>("Turret");

		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Blue_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->AddComponent(std::move(turretTexture));

		auto turretTransform = turret->GetComponent<TransformComponent>();
		turretTransform->SetPosition({ 0, 3 });
		turretTransform->OffsetPivotPoint({ 0, .2f });

		float shootAllowanceRange = 28.f;

		auto shootComponent = std::make_unique<ShootComponent>(turret.get(), shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		tank->AddChild(std::move(turret));

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateYellowTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);
		GameAdmin::Instance().RegisterPlayer(tank.get());

		auto observer = std::make_unique<TankObserver>();
		tank->GetSubject().AddObserver(observer.get());
		tank->AddComponent(std::move(observer));

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/YellowTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->AddComponent(std::move(tankTexture));
		tank->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		auto playerHealth = std::make_unique<HealthComponent>(3);
		tank->GetSubject().AddObserver(playerHealth.get());
		tank->AddComponent(std::move(playerHealth));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->GetSubject().AddObserver(playerScore.get());
		tank->AddComponent(std::move(playerScore));

		auto turret = std::make_unique<GameObject>("Turret");

		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Blue_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->AddComponent(std::move(turretTexture));

		auto turretTransform = turret->GetComponent<TransformComponent>();
		turretTransform->SetPosition({ 0, 3 });
		turretTransform->OffsetPivotPoint({ 0, .2f });

		float shootAllowanceRange = 28.f;

		auto shootComponent = std::make_unique<ShootComponent>(turret.get(), shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		tank->AddChild(std::move(turret));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

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

		auto bulletMoveComponent = std::make_unique<BulletMoveComponent>(moveDirection, 250.f);
		bullet->AddComponent(std::move(bulletMoveComponent));

		auto bulletBehavior = std::make_unique<BulletCollisionBehaviorComponent>();
		bullet->AddComponent(std::move(bulletBehavior));

		auto bulletCollider = std::make_unique<BoxCollider>(SDL_Rect{ 0, 0,bullet->GetComponent<TextureComponent>()->GetDefaultWidth(),bullet->GetComponent<TextureComponent>()->GetDefaultHeight() });
		bulletCollider->isTrigger = true;


		GameObject* bulletRaw = bullet.get();
		bulletCollider->OnTrigger = [bulletRaw](Collider* other)
			{
				auto behavior = bulletRaw->GetComponent<BulletCollisionBehaviorComponent>();
				if (behavior)
					behavior->OnTrigger(bulletRaw, other);
			};

		bullet->AddComponent(std::move(bulletCollider));

		return bullet;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateEnemyBulletPrefab(glm::vec2 spawnPosition, glm::vec2 moveDirection, const std::string tag) const
	{
		auto bullet = std::make_unique<GameObject>(tag);

		auto bulletTexture = std::make_unique<TextureComponent>("data/artassets/EnemyBullet.png", SceneManager::Instance().GetRenderer());
		auto bulletTransform = bullet->GetComponent<TransformComponent>();

		bulletTransform->CentralizePivotOnTexture(bulletTexture.get());
		bullet->AddComponent(std::move(bulletTexture));

		bullet->GetComponent<TransformComponent>()->SetPosition(spawnPosition - bulletTransform->GetPivot());

		auto bulletMoveComponent = std::make_unique<BulletMoveComponent>(moveDirection, 250.f, 0);
		bullet->AddComponent(std::move(bulletMoveComponent));

		auto bulletBehavior = std::make_unique<BulletCollisionBehaviorComponent>();
		bullet->AddComponent(std::move(bulletBehavior));

		auto bulletCollider = std::make_unique<BoxCollider>(SDL_Rect{ 0, 0,bullet->GetComponent<TextureComponent>()->GetDefaultWidth(),bullet->GetComponent<TextureComponent>()->GetDefaultHeight() });
		bulletCollider->isTrigger = true;

		GameObject* bulletRaw = bullet.get();
		bulletCollider->OnTrigger = [bulletRaw](Collider* other)
			{
				auto behavior = bulletRaw->GetComponent<BulletCollisionBehaviorComponent>();
				if (behavior)
					behavior->OnTrigger(bulletRaw, other);
			};

		bullet->AddComponent(std::move(bulletCollider));

		return bullet;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateTeleportCenterPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tpCenter = std::make_unique<GameObject>(tag);
		spawnPosition = { 478,394 };
		tpCenter->GetComponent<TransformComponent>()->SetPosition(spawnPosition);
		tpCenter->GetComponent<TransformComponent>()->SetPivot({0,0});

		int tpBoxWidth = 68;
		int tpBoxHeight = 66;

		auto centerTrigger = std::make_unique<BoxCollider>(SDL_Rect{ 0, 0, tpBoxWidth, tpBoxHeight });
		centerTrigger->isTrigger = true;

		auto teleportComponent = std::make_unique<TeleportTriggerComponent>();

		teleportComponent->Initialize(centerTrigger.get());

		tpCenter->AddComponent(std::move(centerTrigger));
		tpCenter->AddComponent(std::move(teleportComponent));

		return tpCenter;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateHealthUIForPlayer1(glm::vec2 spawnPosition, int maxHealth, const std::string tag) const
	{
		int numberOffset = 30;
		auto healthUI = std::make_unique<GameObject>(tag);
		auto healthUITextComponent = std::make_unique<TextComponent>("Health P1", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 0,0,255,255 }, SceneManager::Instance().GetRenderer());

		healthUI->AddComponent(std::move(healthUITextComponent));
		healthUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x,spawnPosition.y });


		auto healthUIPlayer1 = std::make_unique<GameObject>(tag);
		auto healthUIComponent = std::make_unique<HealthUIComponent>(maxHealth);
		healthUIPlayer1->AddComponent(std::move(healthUIComponent));
		healthUIPlayer1->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer1->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });
		SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1")->GetSubject().AddObserver(healthUIPlayer1->GetComponent<HealthUIComponent>());

		healthUI->AddChild(std::move(healthUIPlayer1));
		return healthUI;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateHealthUIForPlayer2(glm::vec2 spawnPosition, int maxHealth, const std::string tag) const
	{
		int numberOffset = 30;
		auto healthUI = std::make_unique<GameObject>(tag);
		auto healthUITextComponent = std::make_unique<TextComponent>("Health P2", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,0,0,255 }, SceneManager::Instance().GetRenderer());

		healthUI->AddComponent(std::move(healthUITextComponent));
		healthUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y });


		auto healthUIPlayer2 = std::make_unique<GameObject>(tag);
		auto healthUIComponent = std::make_unique<HealthUIComponent>(maxHealth, SDL_Color(255, 0, 0, 255));
		healthUIPlayer2->AddComponent(std::move(healthUIComponent));
		healthUIPlayer2->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer2->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });
		SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player2")->GetSubject().AddObserver(healthUIPlayer2->GetComponent<HealthUIComponent>());

		healthUI->AddChild(std::move(healthUIPlayer2));
		return healthUI;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateHighScoreUI(glm::vec2 spawnPosition, const std::string tag) const
	{
		int numberOffset = 30;
		auto highScoreUIText = std::make_unique<GameObject>(tag);
		auto highScoreTextComponent = std::make_unique<TextComponent>("Highscore", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,255,0,255 }, SceneManager::Instance().GetRenderer());

		highScoreUIText->AddComponent(std::move(highScoreTextComponent));
		highScoreUIText->GetComponent<TransformComponent>()->SetPosition(spawnPosition);


		auto highScoreUI = std::make_unique<GameObject>(tag);
		auto highScoreUITextComponent = std::make_unique<ScoreUIComponent>(HighscoreManager("data/highscores.txt").GetHighestScore());
		highScoreUI->AddComponent(std::move(highScoreUITextComponent));
		highScoreUI->GetComponent<ScoreUIComponent>()->Initialize();
		highScoreUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });

		highScoreUIText->AddChild(std::move(highScoreUI));

		return highScoreUIText;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateCurrentScoreUI(glm::vec2 spawnPosition, const std::string tag) const
	{
		int numberOffset = 30;
		auto currentScoreUIText = std::make_unique<GameObject>(tag);
		auto highScoreTextComponent = std::make_unique<TextComponent>("Current score", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,255,0,255 }, SceneManager::Instance().GetRenderer());

		currentScoreUIText->AddComponent(std::move(highScoreTextComponent));
		currentScoreUIText->GetComponent<TransformComponent>()->SetPosition(spawnPosition);


		auto currentScoreUI = std::make_unique<GameObject>(tag);
		auto currentScoreUITextComponent = std::make_unique<ScoreUIComponent>(GameData::CurrentScore);
		currentScoreUI->AddComponent(std::move(currentScoreUITextComponent));
		currentScoreUI->GetComponent<ScoreUIComponent>()->Initialize();
		currentScoreUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });
		SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1")->GetSubject().AddObserver(currentScoreUI->GetComponent<ScoreUIComponent>());

		currentScoreUIText->AddChild(std::move(currentScoreUI));

		return currentScoreUIText;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateEnemyManager(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto enemyManager = std::make_unique<GameObject>("EnemyManager");

		auto managerComponent = std::make_unique<EnemyManagerComponent>();
		enemyManager->AddComponent(std::move(managerComponent));

		return enemyManager;
	}

}

