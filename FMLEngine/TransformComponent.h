#pragma once
#include "Component.h"

class TransformComponent : public Component {
public:
    TransformComponent(float x = 0.0f, float y = 0.0f, float width = 0.0f, float height = 0.0f)
        : x(x), y(y), width(width), height(height) {
    }

    void SetPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void GetPosition(float& outX, float& outY) const {
        outX = x;
        outY = y;
    }

    void SetSize(float newWidth, float newHeight) {
        width = newWidth;
        height = newHeight;
    }

    float GetX() const { return x; }
    float GetY() const { return y; }
    float GetWidth() const { return width; }
    float GetHeight() const { return height; }

    bool IsSizeSet() const {
        return width > 0.0f && height > 0.0f;
    }

private:
    float x, y;
    float width, height;
};
