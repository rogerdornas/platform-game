//
// Created by roger on 02/09/2025.
//

#pragma once

#include "Actor.h"

class JumpEffect : public Actor
{
public:
    enum class EffectType{
        TakeOff,
        Land,
        DoubleJump
    };

    JumpEffect(class Game* game, Actor* owner, float duration);

    void OnUpdate(float deltaTime) override;
    void StartEffect(EffectType type);
    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ManageAnimations();

    float mWidth;
    float mHeight;
    float mEffectDuration;
    float mEffectTimer;
    Actor* mOwner;

    class AnimatorComponent* mDrawComponent;
};

