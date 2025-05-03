#include "SpriteAnimatorComponent.h"
#include "TextureComponent.h"
#include "GameObject.h"
#include <stdexcept>

namespace FML
{
    SpriteAnimatorComponent::SpriteAnimatorComponent(int frameWidth, int frameHeight, int totalFrames, float frameTime)
        : frameWidth(frameWidth),
        frameHeight(frameHeight),
        totalFrames(totalFrames),
        frameTime(frameTime),
        currentFrame(0),
        timeAccumulator(0.0f),
        isPlaying(false),
        isLooping(true),
        textureComponent(nullptr)
    {
    }

    void SpriteAnimatorComponent::Initialize()
    {
        textureComponent = gameObject->GetComponent<TextureComponent>();
        if (!textureComponent)
        {
            throw std::runtime_error("SpriteAnimatorComponent requires a TextureComponent.");
        }

        // Set initial frame
        textureComponent->SetSourceRect({ 0, 0, frameWidth, frameHeight });
    }

    void SpriteAnimatorComponent::Update(float deltaTime)
    {
        if (!isPlaying || totalFrames <= 1)
            return;

        timeAccumulator += deltaTime;
        if (timeAccumulator >= frameTime)
        {
            timeAccumulator -= frameTime;
            currentFrame++;
            if (currentFrame >= totalFrames)
            {
                if (isLooping)
                    currentFrame = 0;
                else
                {
                    currentFrame = totalFrames - 1;
                    isPlaying = false;
                }
            }

            int textureWidth = textureComponent->GetDefaultWidth();
            int columns = textureWidth / frameWidth;

            int x = (currentFrame % columns) * frameWidth;
            int y = (currentFrame / columns) * frameHeight;

            textureComponent->SetSourceRect({ x, y, frameWidth, frameHeight });
        }
    }

    void SpriteAnimatorComponent::Play()
    {
        isPlaying = true;
    }

    void SpriteAnimatorComponent::Stop()
    {
        isPlaying = false;
    }

    void SpriteAnimatorComponent::SetLooping(bool loop)
    {
        isLooping = loop;
    }
}
