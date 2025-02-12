#include "TestingScene.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputHandler.h"
#include "ConfigManager.h"
#include "TextComponent.h"
#include "FPSComponent.h"

const std::string backgroundImagePath = "data/artassets/tron_bg.png";

bool TestingScene::Initialize(SDL_Renderer* renderer) {
	InitializeBackground(renderer);
    InitializeTitle(renderer);
    InitializeFPSCounter(renderer);

	return true;
}

void TestingScene::InitializeBackground(SDL_Renderer* renderer) {
    auto background = std::make_unique<GameObject>();
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

    titleTextComponent->SetPosition(300, 300);

    title->AddComponent(std::move(titleTextComponent));

    gameObjects.push_back(std::move(title));
}

void TestingScene::InitializeFPSCounter(SDL_Renderer* renderer)
{
    auto fpsGameObject = std::make_unique<GameObject>("FPSCounter");

    SDL_Color fpsColor = { 255, 255, 255, 255 };
    auto fpsComponent = std::make_unique<FPSComponent>(renderer, "data/fonts/tron-arcade.ttf", 18, fpsColor);
    fpsGameObject->GetComponent<TransformComponent>()->SetPosition(10, 10);

    fpsGameObject->AddComponent(std::move(fpsComponent));
    gameObjects.push_back(std::move(fpsGameObject));
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
