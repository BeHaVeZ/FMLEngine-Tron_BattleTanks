#include "TestingScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "RotationComponent.h"
#include <iostream>

const std::string backgroundImagePath = "data/artassets/tron_bg.png";

bool TestingScene::Initialize(SDL_Renderer* renderer) {
	//InitializeBackground(renderer);
    //InitializeTitle(renderer);
    InitializeFPSCounter(renderer);

	InitializeFirstTank(renderer);
	InitializeSecondTank(renderer);

	return true;
}

void TestingScene::InitializeBackground(SDL_Renderer* renderer) {
    auto background = std::make_unique<GameObject>("Background");
    auto backgroundTexture = std::make_unique<TextureComponent>(backgroundImagePath, renderer);
    background->AddComponent(std::move(backgroundTexture));

    auto backgroundTransform = background->GetComponent<TransformComponent>();
    if (backgroundTransform) {
        backgroundTransform->SetPosition(0, 0);
        backgroundTransform->SetSize(
            static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
            static_cast<float>(ConfigManager::Instance().GetWindowHeight())
        );
    }
    gameObjects.push_back(std::move(background));
}

void TestingScene::InitializeTitle(SDL_Renderer* renderer)
{
    auto title = std::make_unique<GameObject>("title");

    SDL_Color color = { 0, 255, 0, 255 };

    auto titleTextComponent = std::make_unique<TextComponent>(
        "TRON Battle Tanks",         
        "data/fonts/tron-arcade.ttf",      
        24,                          
        color,                       
        renderer);                   

    title->AddComponent(std::move(titleTextComponent));
	title->GetComponent<TransformComponent>()->SetPosition(300, 300);

    gameObjects.push_back(std::move(title));
}

void TestingScene::InitializeFPSCounter(SDL_Renderer* renderer)
{
    auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

    SDL_Color fpsColor = { 255, 255, 255, 255 };

    auto titleTextComponent = std::make_unique<TextComponent>(
        "FPS 0",
        "data/fonts/tron-arcade.ttf",
        22,
        fpsColor,
        renderer);
    fpsGameObject->AddComponent(std::move(titleTextComponent));


    auto fpsComponent = std::make_unique<FPSComponent>(renderer);
    fpsGameObject->AddComponent(std::move(fpsComponent));
	fpsGameObject->GetComponent<FPSComponent>()->Initialize();

    fpsGameObject->GetComponent<TransformComponent>()->SetPosition(10, 10);

    gameObjects.push_back(std::move(fpsGameObject));
}

void TestingScene::InitializeFirstTank(SDL_Renderer* renderer)
{
	auto tank = std::make_unique<GameObject>("Tank1");

	auto tankTexture = std::make_unique<TextureComponent>("data/artassets/RedTank.png", renderer);
	tank->AddComponent(std::move(tankTexture));

	tank->GetComponent<TransformComponent>()->SetPosition(500, 500);

	auto rotationComponent = std::make_unique<RotationComponent>(20.f, 200.f, 500.f, 300.f);
	tank->AddComponent(std::move(rotationComponent));
	tank->GetComponent<RotationComponent>()->Initialize();

	gameObjects.push_back(std::move(tank));
}

void TestingScene::InitializeSecondTank(SDL_Renderer* renderer)
{
    auto tank = std::make_unique<GameObject>("Tank2");

    auto tankTexture = std::make_unique<TextureComponent>("data/artassets/BlueTank.png", renderer);
    tank->AddComponent(std::move(tankTexture));
    tank->GetComponent<TransformComponent>()->SetPosition(100, 100);


    auto rotationComponent = std::make_unique<RotationComponent>(100.f, 500.f);
    tank->AddComponent(std::move(rotationComponent));
    tank->GetComponent<RotationComponent>()->Initialize();

	FindGameObjectByTag("Tank1")->AddChild(std::move(tank));
    gameObjects;
}


void TestingScene::HandleInput(SDL_Event& event) {
	InputHandler::Instance().HandleInput(event);
}

void TestingScene::Update(float deltaTime) {
	for (auto& gameObject : gameObjects) {
		gameObject->Update(deltaTime);
	}
}

void TestingScene::Render(SDL_Renderer* renderer) {
	SDL_RenderClear(renderer);

	for (auto& gameObject : gameObjects) {
		gameObject->Render(renderer);
	}

	SDL_RenderPresent(renderer);
}

void TestingScene::Cleanup() {
}
