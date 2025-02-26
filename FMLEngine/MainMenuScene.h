#pragma once
#include "Scene.h"
#include "MenuInterface.h"
#include "MenuManager.h"
#include <memory>
#include <vector>

class MainMenuScene : public Scene, public MenuInterface {
public:
    MainMenuScene() : storedRenderer(nullptr), selectedIndex(0) {}

    bool Initialize(SDL_Renderer* renderer) override;
    void HandleInput(SDL_Event& event) override;
    void Update(float deltaTime) override;
    void Render(SDL_Renderer* renderer) override;
    void Cleanup() override;

    void InitializeBackground(SDL_Renderer* renderer);
    void InitializeMenuOptions(SDL_Renderer* renderer);
    void InitializeSelectionArrow(SDL_Renderer* renderer);

    void InitializeSounds();

    void SelectTopItem() override;
    void SelectBottomItem() override;
    void SelectLeftItem() override;
    void SelectRightItem() override;

private:
    std::vector<GameObject*> menuOptions;
    std::unique_ptr<GameObject> selectionArrow;
    int selectedIndex;

    SDL_Renderer* storedRenderer;
};
