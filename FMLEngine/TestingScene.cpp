#include "TestingScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"
#include "RotationComponent.h"
#include "ServiceLocator.h"
#include <iostream>
#include "MuteSoundCommand.h"
#include "MoveCommand.h"

const std::string backgroundImagePath = "data/levels/level00.png";

bool TestingScene::Initialize(SDL_Renderer* renderer) {
	InitializeBackground(renderer);
    InitializeFPSCounter(renderer);

	InitializeFirstTank(renderer);
	//InitializeSecondTank(renderer);

    InitializeInput();
    InitializeSounds();

	return true;
}

void TestingScene::InitializeBackground(SDL_Renderer* renderer) {
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
    title->GetComponent<TransformComponent>()->SetPosition({ 300, 300 });

    gameObjects.push_back(std::move(title));
}

void TestingScene::InitializeFPSCounter(SDL_Renderer* renderer)
{
    auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

    SDL_Color fpsColor = { 255, 255, 255, 255 };

    auto titleTextComponent = std::make_unique<TextComponent>(
        "FPS 0",
        "data/fonts/tron-arcade.ttf",
        10,
        fpsColor,
        renderer);
    fpsGameObject->AddComponent(std::move(titleTextComponent));


    auto fpsComponent = std::make_unique<FPSComponent>(renderer);
    fpsGameObject->AddComponent(std::move(fpsComponent));
	fpsGameObject->GetComponent<FPSComponent>()->Initialize();

    fpsGameObject->GetComponent<TransformComponent>()->SetPosition({ 10, 10 });

    gameObjects.push_back(std::move(fpsGameObject));
}

void TestingScene::InitializeFirstTank(SDL_Renderer* renderer)
{
	auto tank = std::make_unique<GameObject>("Tank1");

	auto tankTexture = std::make_unique<TextureComponent>("data/artassets/RedTank.png", renderer);
	tank->AddComponent(std::move(tankTexture));

    tank->GetComponent<TransformComponent>()->SetPosition({ 500, 500 });

	gameObjects.push_back(std::move(tank));
}

void TestingScene::InitializeSecondTank(SDL_Renderer* renderer)
{
    auto tank = std::make_unique<GameObject>("Tank2");

    auto tankTexture = std::make_unique<TextureComponent>("data/artassets/BlueTank.png", renderer);
    tank->AddComponent(std::move(tankTexture));
    tank->GetComponent<TransformComponent>()->SetPosition({ 100, 100 });


    auto rotationComponent = std::make_unique<RotationComponent>(100.f, 500.f);
    tank->AddComponent(std::move(rotationComponent));
    tank->GetComponent<RotationComponent>()->Initialize();

	FindGameObjectByTag("Tank1")->AddChild(std::move(tank));
}

void TestingScene::InitializeInput() {
    auto tank1 = FindGameObjectByTag("Tank1");
    if (tank1) 
    {
        InputHandler::Instance().BindCommand(SDLK_w, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 100.f));
        InputHandler::Instance().BindCommand(SDLK_s, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 100.f));
        InputHandler::Instance().BindCommand(SDLK_a, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 100.f));
        InputHandler::Instance().BindCommand(SDLK_d, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 100.f));

        int controllerId = 0;
        InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_UP, std::make_unique<MoveCommand>(tank1, glm::vec2(0, -1), 200.f));
        InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_DOWN, std::make_unique<MoveCommand>(tank1, glm::vec2(0, 1), 200.f));
        InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_LEFT, std::make_unique<MoveCommand>(tank1, glm::vec2(-1, 0), 200.f));
        InputHandler::Instance().BindGamepadCommand(controllerId, XINPUT_GAMEPAD_DPAD_RIGHT, std::make_unique<MoveCommand>(tank1, glm::vec2(1, 0), 200.f));
    }
    InputHandler::Instance().BindGamepadCommand(0, XINPUT_GAMEPAD_Y, std::make_unique<MuteSoundCommand>(),InputHandler::KeyAction::KeyUp);
}

void TestingScene::InitializeSounds()
{
	ServiceLocator::GetSoundSystem().AddSound("Game_Music.mp3",1,true);
	ServiceLocator::GetSoundSystem().PlaySound(1, 1.f);
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
