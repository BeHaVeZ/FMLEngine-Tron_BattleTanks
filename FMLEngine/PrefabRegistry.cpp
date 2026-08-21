#include "PrefabRegistry.h"
#include "TextureComponent.h"
#include "SceneManager.h"
#include "HealthComponent.h"
#include "HealthUIComponent.h"
#include "Collider.h"
#include "BoxCollider.h"
#include "TurretAimComponent.h"
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
#include <SpriteAnimatorComponent.h>
#include "../Tron_BattleTanks/BulletObserver.h"
#include "../Tron_BattleTanks/RecognizerCollisionBehaviorComponent.h"
#include "../Tron_BattleTanks/GameData.h"
#include "../Tron_BattleTanks/RecognizerStateComponent.h"
#include "../Tron_BattleTanks/NormalMovingState.h"
#include "../Tron_BattleTanks/PlayerRespawnComponent.h"
#include "../Tron_BattleTanks/InvulnerabilityFlickerComponent.h"
#include "../Tron_BattleTanks/LevelProgressionComponent.h"
#include "../Tron_BattleTanks/FloatingScoreComponent.h"


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

		auto playerHealth = std::make_unique<HealthComponent>(&GameData::Player1Health);
		tank->GetSubject().AddObserver(playerHealth.get());
		tank->AddComponent(std::move(playerHealth));

		auto respawn = std::make_unique<PlayerRespawnComponent>();
		tank->GetSubject().AddObserver(respawn.get());
		tank->AddComponent(std::move(respawn));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->AddComponent(std::move(playerScore));
		tank->GetComponent<ScoreComponent>()->Initialize();

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

		auto shootComponent = std::make_unique<ShootComponent>(shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		turret->AddComponent(std::make_unique<TurretAimComponent>());

		tank->AddChild(std::move(turret));

		auto flicker = std::make_unique<InvulnerabilityFlickerComponent>();
		tank->AddComponent(std::move(flicker));
		tank->GetComponent<InvulnerabilityFlickerComponent>()->Initialize();

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

	std::unique_ptr<GameObject> PrefabRegistry::CreateTargetDummyPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto dummy = std::make_unique<GameObject>(tag);

		auto observer = std::make_unique<TankObserver>();
		dummy->GetSubject().AddObserver(observer.get());
		dummy->AddComponent(std::move(observer));

		auto dummyTexture = std::make_unique<TextureComponent>("data/artassets/GreenTank.png", SceneManager::Instance().GetRenderer());
		dummy->GetComponent<TransformComponent>()->CentralizePivotOnTexture(dummyTexture.get());
		dummy->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y });
		dummy->AddComponent(std::move(dummyTexture));

		auto dummyHealth = std::make_unique<HealthComponent>(1);
		dummy->GetSubject().AddObserver(dummyHealth.get());
		dummy->AddComponent(std::move(dummyHealth));

		SDL_Rect dummyBox = { 0,0,dummy->GetComponent<TextureComponent>()->GetDefaultWidth() - 2,dummy->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto dummyCollider = std::make_unique<BoxCollider>(dummyBox);
		dummy->AddComponent(std::move(dummyCollider));

		return dummy;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateRecognizerPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto recognizer = std::make_unique<GameObject>(tag);

		auto observer = std::make_unique<TankObserver>();
		recognizer->GetSubject().AddObserver(observer.get());
		recognizer->AddComponent(std::move(observer));

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/Recognizer.png", SceneManager::Instance().GetRenderer());
		recognizer->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		recognizer->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y });
		recognizer->AddComponent(std::move(tankTexture));

		auto tankHealth = std::make_unique<HealthComponent>(1);
		recognizer->GetSubject().AddObserver(tankHealth.get());
		recognizer->AddComponent(std::move(tankHealth));

		auto stateComponent = std::make_unique<RecognizerStateComponent>();
		recognizer->AddComponent(std::move(stateComponent));
		recognizer->GetComponent<RecognizerStateComponent>()->ChangeState(std::make_unique<NormalMovingState>());

		auto recognizerCollisionBehavior = std::make_unique<RecognizerCollisionBehaviorComponent>();
		recognizer->AddComponent(std::move(recognizerCollisionBehavior));

		SDL_Rect recognizerBox = { 0,0,recognizer->GetComponent<TextureComponent>()->GetDefaultWidth() - 2,recognizer->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto recognizerCollider = std::make_unique<BoxCollider>(recognizerBox);

		GameObject* recognizerRaw = recognizer.get();
		recognizerCollider->OnCollision = [recognizerRaw](Collider* other)
			{
				auto behavior = recognizerRaw->GetComponent<RecognizerCollisionBehaviorComponent>();
				if (behavior)
					behavior->OnCollision(recognizerRaw, other);
			};
		recognizer->AddComponent(std::move(recognizerCollider));

		return recognizer;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreatePinkTankPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto tank = std::make_unique<GameObject>(tag);

		auto observer = std::make_unique<TankObserver>();
		tank->GetSubject().AddObserver(observer.get());
		tank->AddComponent(std::move(observer));

		auto tankTexture = std::make_unique<TextureComponent>("data/artassets/PinkTank.png", SceneManager::Instance().GetRenderer());
		tank->GetComponent<TransformComponent>()->CentralizePivotOnTexture(tankTexture.get());
		tank->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y});
		tank->AddComponent(std::move(tankTexture));

		auto tankHealth = std::make_unique<HealthComponent>(1);
		tank->GetSubject().AddObserver(tankHealth.get());
		tank->AddComponent(std::move(tankHealth));

		auto enemyMovement = std::make_unique<EnemyMovementComponent>(65.f);
		tank->AddComponent(std::move(enemyMovement));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth() - 2,tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

		auto shootComponent = std::make_unique<EnemyShootComponent>();
		shootComponent->SetBulletSpeed(500.f);
		tank->AddComponent(std::move(shootComponent));

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

		auto respawn = std::make_unique<PlayerRespawnComponent>();
		tank->GetSubject().AddObserver(respawn.get());
		tank->AddComponent(std::move(respawn));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->AddComponent(std::move(playerScore));
		tank->GetComponent<ScoreComponent>()->Initialize();

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

		auto shootComponent = std::make_unique<ShootComponent>(shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		turret->AddComponent(std::make_unique<TurretAimComponent>());

		tank->AddChild(std::move(turret));

		auto flicker = std::make_unique<InvulnerabilityFlickerComponent>();
		tank->AddComponent(std::move(flicker));
		tank->GetComponent<InvulnerabilityFlickerComponent>()->Initialize();

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

		auto playerHealth = std::make_unique<HealthComponent>(&GameData::Player2Health);
		tank->GetSubject().AddObserver(playerHealth.get());
		tank->AddComponent(std::move(playerHealth));

		auto respawn = std::make_unique<PlayerRespawnComponent>();
		tank->GetSubject().AddObserver(respawn.get());
		tank->AddComponent(std::move(respawn));

		auto playerScore = std::make_unique<ScoreComponent>();
		tank->AddComponent(std::move(playerScore));
		tank->GetComponent<ScoreComponent>()->Initialize();

		auto turret = std::make_unique<GameObject>("Turret");

		auto turretTexture = std::make_unique<TextureComponent>("data/artassets/Blue_Barrel.png", SceneManager::Instance().GetRenderer());
		turret->AddComponent(std::move(turretTexture));

		auto turretTransform = turret->GetComponent<TransformComponent>();
		turretTransform->SetPosition({ 0, 3 });
		turretTransform->OffsetPivotPoint({ 0, .2f });

		float shootAllowanceRange = 28.f;

		auto shootComponent = std::make_unique<ShootComponent>(shootAllowanceRange, .5f);
		turret->AddComponent(std::move(shootComponent));

		turret->AddComponent(std::make_unique<TurretAimComponent>());

		tank->AddChild(std::move(turret));

		SDL_Rect tankBox = { 0,0,tank->GetComponent<TextureComponent>()->GetDefaultWidth(),tank->GetComponent<TextureComponent>()->GetDefaultHeight() };
		auto playerCollider = std::make_unique<BoxCollider>(tankBox);
		tank->AddComponent(std::move(playerCollider));

		auto flicker = std::make_unique<InvulnerabilityFlickerComponent>();
		tank->AddComponent(std::move(flicker));
		tank->GetComponent<InvulnerabilityFlickerComponent>()->Initialize();

		return tank;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateBulletPrefab(glm::vec2 spawnPosition, glm::vec2 moveDirection, const std::string tag, int ownerPlayer) const
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
		bulletBehavior->SetOwnerPlayer(ownerPlayer);
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

		bullet->GetComponent<BoxCollider>()->SyncToTransform();

		auto observer = std::make_unique<BulletObserver>();
		bullet->GetSubject().AddObserver(observer.get());
		bullet->AddComponent(std::move(observer));

		return bullet;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateEnemyBulletPrefab(glm::vec2 spawnPosition, glm::vec2 moveDirection,float bulletSpeed, const std::string tag) const
	{
		auto bullet = std::make_unique<GameObject>(tag);

		auto bulletTexture = std::make_unique<TextureComponent>("data/artassets/EnemyBullet.png", SceneManager::Instance().GetRenderer());
		auto bulletTransform = bullet->GetComponent<TransformComponent>();

		bulletTransform->CentralizePivotOnTexture(bulletTexture.get());
		bullet->AddComponent(std::move(bulletTexture));

		bullet->GetComponent<TransformComponent>()->SetPosition(spawnPosition - bulletTransform->GetPivot());

		auto bulletMoveComponent = std::make_unique<BulletMoveComponent>(moveDirection, bulletSpeed, 0);
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

		bullet->GetComponent<BoxCollider>()->SyncToTransform();

		auto observer = std::make_unique<BulletObserver>();
		bullet->GetSubject().AddObserver(observer.get());
		bullet->AddComponent(std::move(observer));

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

	std::unique_ptr<GameObject> PrefabRegistry::CreateHealthUIForPlayer1(glm::vec2 spawnPosition, const std::string tag) const
	{
		int numberOffset = 30;
		auto healthUI = std::make_unique<GameObject>(tag);
		auto healthUITextComponent = std::make_unique<TextComponent>("Lives P1", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 0,0,255,255 }, SceneManager::Instance().GetRenderer());

		healthUI->AddComponent(std::move(healthUITextComponent));
		healthUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x,spawnPosition.y });


		GameObject* player1 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player1");

		auto healthUIPlayer1 = std::make_unique<GameObject>(tag);
		auto healthUIComponent = std::make_unique<HealthUIComponent>(player1->GetComponent<HealthComponent>());
		healthUIPlayer1->AddComponent(std::move(healthUIComponent));
		healthUIPlayer1->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer1->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });
		player1->GetSubject().AddObserver(healthUIPlayer1->GetComponent<HealthUIComponent>());

		healthUI->AddChild(std::move(healthUIPlayer1));
		return healthUI;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateHealthUIForPlayer2(glm::vec2 spawnPosition, const std::string tag) const
	{
		int numberOffset = 30;
		auto healthUI = std::make_unique<GameObject>(tag);
		auto healthUITextComponent = std::make_unique<TextComponent>("Lives P2", "data/fonts/tron-arcade.ttf", 20, SDL_Color{ 255,0,0,255 }, SceneManager::Instance().GetRenderer());

		healthUI->AddComponent(std::move(healthUITextComponent));
		healthUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y });


		GameObject* player2 = SceneManager::Instance().GetCurrentScene()->FindGameObjectByTag("Player2");

		auto healthUIPlayer2 = std::make_unique<GameObject>(tag);
		auto healthUIComponent = std::make_unique<HealthUIComponent>(player2->GetComponent<HealthComponent>(), SDL_Color(255, 0, 0, 255));
		healthUIPlayer2->AddComponent(std::move(healthUIComponent));
		healthUIPlayer2->GetComponent<HealthUIComponent>()->Initialize();
		healthUIPlayer2->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });
		player2->GetSubject().AddObserver(healthUIPlayer2->GetComponent<HealthUIComponent>());

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

	std::unique_ptr<GameObject> PrefabRegistry::CreateScoreUI(glm::vec2 spawnPosition, const std::string& label, const int* source, SDL_Color color, const std::string tag) const
	{
		int numberOffset = 30;
		auto scoreUIText = std::make_unique<GameObject>(tag);
		auto labelTextComponent = std::make_unique<TextComponent>(label, "data/fonts/tron-arcade.ttf", 20, color, SceneManager::Instance().GetRenderer());

		scoreUIText->AddComponent(std::move(labelTextComponent));
		scoreUIText->GetComponent<TransformComponent>()->SetPosition(spawnPosition);


		auto scoreUI = std::make_unique<GameObject>(tag);
		auto scoreUIComponent = std::make_unique<ScoreUIComponent>(source, color);
		scoreUI->AddComponent(std::move(scoreUIComponent));
		scoreUI->GetComponent<ScoreUIComponent>()->Initialize();
		scoreUI->GetComponent<TransformComponent>()->SetPosition({ spawnPosition.x, spawnPosition.y + numberOffset });

		scoreUIText->AddChild(std::move(scoreUI));

		return scoreUIText;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateCurrentScoreUI(glm::vec2 spawnPosition, const std::string tag) const
	{
		return CreateScoreUI(spawnPosition, "Current score", &GameData::CurrentScore, SDL_Color{ 255,255,0,255 }, tag);
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateFloatingScorePrefab(glm::vec2 spawnPosition, int score, const std::string tag) const
	{
		auto floatingScore = std::make_unique<GameObject>(tag);

		auto text = std::make_unique<TextComponent>(std::to_string(score), "data/fonts/tron-arcade.ttf", 16, SDL_Color{ 255, 255, 0, 255 }, SceneManager::Instance().GetRenderer());

		const glm::vec2 origin{ spawnPosition.x - text->GetWidth() * .5f, spawnPosition.y - text->GetHeight() * .5f };

		floatingScore->AddComponent(std::move(text));
		floatingScore->AddComponent(std::make_unique<FloatingScoreComponent>(origin));
		floatingScore->GetComponent<TransformComponent>()->SetPosition(origin);
		floatingScore->Initialize();

		return floatingScore;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateEnemyManager(glm::vec2, const std::string) const
	{
		auto enemyManager = std::make_unique<GameObject>("EnemyManager");
		enemyManager->AddComponent(std::make_unique<LevelProgressionComponent>());

		auto managerComponent = std::make_unique<EnemyManagerComponent>();
		enemyManager->AddComponent(std::move(managerComponent));

		return enemyManager;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateTankExplosionPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto explosion = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/tankExplosion_ss.png", SceneManager::Instance().GetRenderer());

		explosion->AddComponent(std::move(texture));

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			48, 48,
			7, 
			0.10f
		);
		explosion->AddComponent(std::move(animator));

		explosion->Initialize();

		explosion->GetComponent<TransformComponent>()->SetSize(84,84);
		explosion->GetComponent<SpriteAnimatorComponent>()->Play();

		explosion->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return explosion;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateBulletExplosionPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto explosion = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/bulletExplosion_ss.png", SceneManager::Instance().GetRenderer());
		explosion->AddComponent(std::move(texture));

		explosion->GetComponent<TransformComponent>()->SetSize(32, 32);

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			48, 48,
			8,
			0.05f
		);
		explosion->AddComponent(std::move(animator));

		explosion->Initialize();
		explosion->GetComponent<SpriteAnimatorComponent>()->Play();

		explosion->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return explosion;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateHitExplosionPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto explosion = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/hitExplosion_ss.png", SceneManager::Instance().GetRenderer());
		explosion->AddComponent(std::move(texture));

		explosion->GetComponent<TransformComponent>()->SetSize(32, 32);

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			32, 32,
			8,
			0.06f
		);
		explosion->AddComponent(std::move(animator));

		explosion->Initialize();
		explosion->GetComponent<SpriteAnimatorComponent>()->Play();

		explosion->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return explosion;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreatePlayerExplosionPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto explosion = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/playerExplosion_ss.png", SceneManager::Instance().GetRenderer());
		explosion->AddComponent(std::move(texture));

		explosion->GetComponent<TransformComponent>()->SetSize(150, 150);

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			192, 192,
			22,
			0.05f
		);
		explosion->AddComponent(std::move(animator));

		explosion->Initialize();
		explosion->GetComponent<SpriteAnimatorComponent>()->Play();

		explosion->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return explosion;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateTurretShootExplosionPrefab(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto explosion = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/turretFireExplosion_ss.png", SceneManager::Instance().GetRenderer());
		explosion->AddComponent(std::move(texture));

		explosion->GetComponent<TransformComponent>()->SetSize(40, 40);

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			128, 128,
			14,
			0.05f
		);
		explosion->AddComponent(std::move(animator));

		explosion->Initialize();
		explosion->GetComponent<SpriteAnimatorComponent>()->Play();

		explosion->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return explosion;
	}

	std::unique_ptr<GameObject> PrefabRegistry::CreateTpEffect(glm::vec2 spawnPosition, const std::string tag) const
	{
		auto effect = std::make_unique<GameObject>(tag);

		auto texture = std::make_unique<TextureComponent>("data/artassets/tp_ss.png", SceneManager::Instance().GetRenderer());
		effect->AddComponent(std::move(texture));

		effect->GetComponent<TransformComponent>()->SetSize(120, 120);

		auto animator = std::make_unique<SpriteAnimatorComponent>(
			48, 48,
			7,
			0.10f
		);
		effect->AddComponent(std::move(animator));

		effect->Initialize();
		effect->GetComponent<SpriteAnimatorComponent>()->Play();

		effect->GetComponent<TransformComponent>()->SetPosition(spawnPosition);

		return effect;
	}

}
