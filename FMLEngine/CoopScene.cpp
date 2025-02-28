#include "CoopScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "ServiceLocator.h"
#include <iostream>

const std::string backgroundImagePath = "data/artassets/tron_bg.png";

bool CoopScene::Initialize(SDL_Renderer* renderer) {
    InitializeBackground(renderer);
    //InitializeTitle(renderer);
    InitializeFPSCounter(renderer);

    InitializeFirstTank(renderer);
    InitializeSecondTank(renderer);

    InitializeSounds();

    return true;
}

void CoopScene::InitializeBackground(SDL_Renderer* renderer) {
    auto background = std::make_unique<GameObject>("Background");
    auto backgroundTexture = std::make_unique<TextureComponent>(backgroundImagePath, renderer);
    background->AddComponent(std::move(backgroundTexture));

    auto backgroundTransform = background->GetComponent<TransformComponent>();
    if (backgroundTransform) {
        backgroundTransform->SetPosition({ 0, 0 });
        backgroundTransform->SetSize(
            static_cast<float>(ConfigManager::Instance().GetWindowWidth()),
            static_cast<float>(ConfigManager::Instance().GetWindowHeight())
        );
    }
    gameObjects.push_back(std::move(background));
}

void CoopScene::InitializeTitle(SDL_Renderer* renderer)
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
    title->GetComponent<TransformComponent>()->SetPosition({ 300, 300 });

    gameObjects.push_back(std::move(title));
}

void CoopScene::InitializeFPSCounter(SDL_Renderer* renderer)
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

    fpsGameObject->GetComponent<TransformComponent>()->SetPosition({ 10, 10 });

    gameObjects.push_back(std::move(fpsGameObject));
}

void CoopScene::InitializeFirstTank(SDL_Renderer* renderer)
{
    auto tank = std::make_unique<GameObject>("Tank1");

    auto tankTexture = std::make_unique<TextureComponent>("data/artassets/RedTank.png", renderer);
    tank->AddComponent(std::move(tankTexture));

    tank->GetComponent<TransformComponent>()->SetPosition({ 500, 500 });

    gameObjects.push_back(std::move(tank));
}

void CoopScene::InitializeSecondTank(SDL_Renderer* renderer)
{
    auto tank = std::make_unique<GameObject>("Tank2");

    auto tankTexture = std::make_unique<TextureComponent>("data/artassets/BlueTank.png", renderer);
    tank->AddComponent(std::move(tankTexture));
    tank->GetComponent<TransformComponent>()->SetPosition({ 100, 100 });

    FindGameObjectByTag("Tank1")->AddChild(std::move(tank));
    gameObjects;
}

void CoopScene::InitializeSounds()
{
    ServiceLocator::GetSoundSystem().AddSound("Menu_Music.mp3", 1, true);
    ServiceLocator::GetSoundSystem().PlaySound(1, 1.f);
}


void CoopScene::HandleInput(SDL_Event& event) {
    InputHandler::Instance().HandleInput(event);
}

void CoopScene::Update(float deltaTime) {
    for (auto& gameObject : gameObjects) {
        gameObject->Update(deltaTime);
    }
}

void CoopScene::Render(SDL_Renderer* renderer) {
    SDL_RenderClear(renderer);

    for (auto& gameObject : gameObjects) {
        gameObject->Render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void CoopScene::Cleanup() {
}
