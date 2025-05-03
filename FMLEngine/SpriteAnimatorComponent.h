#pragma once
#include "Component.h"
#include "SDL.h"

namespace FML
{
    class TextureComponent;

    class SpriteAnimatorComponent final : public Component
    {
    public:
        SpriteAnimatorComponent(int frameWidth, int frameHeight, int totalFrames, float frameTime);

        void Initialize() override;
        void Update(float deltaTime) override;

        void Play();
        void Stop();
        void SetLooping(bool loop);

    private:
        int frameWidth;
        int frameHeight;
        int totalFrames;
        float frameTime;

        int currentFrame;
        float timeAccumulator;
        bool isPlaying;
        bool isLooping;

        TextureComponent* textureComponent;
    };
}
