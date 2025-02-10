#pragma once

class ConfigManager {
public:
    static ConfigManager& Instance() {
        static ConfigManager instance;
        return instance;
    }

    int GetWindowWidth() const { return windowWidth; }
    int GetWindowHeight() const { return windowHeight; }

    void SetWindowSize(int width, int height) {
        windowWidth = width;
        windowHeight = height;
    }

private:
    ConfigManager() = default;

    int windowWidth = 1024;
    int windowHeight = 768;
};
