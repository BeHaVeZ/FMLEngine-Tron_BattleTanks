#pragma once
#include "MenuInterface.h"

class MenuManager final{
public:
    static MenuManager& Instance() {
        static MenuManager instance;
        return instance;
    }

    void SetCurrentMenu(MenuInterface* menu) {
        currentMenu = menu;
    }

    MenuInterface* GetCurrentMenu() const {
        return currentMenu;
    }

    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;

private:
    MenuManager() = default;

    MenuInterface* currentMenu = nullptr;
};
