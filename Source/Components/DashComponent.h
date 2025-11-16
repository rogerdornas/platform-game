//
// Created by roger on 25/04/2025.
//

#pragma once

#include "Component.h"
#include "../Actors/Actor.h"

class DashEffectActor : public Actor
{
public:
    DashEffectActor(Game* game, Actor* owner, float duration);

    void OnUpdate(float deltaTime) override;
    void StartDashEffect();
    void StopDash() { mEffectTimer = mEffectDuration; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mWidth;
    float mHeight;
    float mEffectDuration;
    float mEffectTimer;
    Actor* mOwner;

    class AnimatorComponent* mDrawComponent;
};


class DashComponent : public Component
{
public:
    DashComponent(class Actor* owner, float dashSpeed, float dashDuration, float dashCooldown);

    bool UseDash(bool isOnGround);
    bool GetIsDashing() const { return mIsDashing; }
    void SetHasDashedInAir(bool hasDashedInAir) { mHasDashedInAir = hasDashedInAir; }

    void StopDash();
    void SetDashSpeed(float dashSpeed) { mDashSpeed = dashSpeed; }
    float GetDashSpeed() const { return mDashSpeed; }

    void Update(float deltaTime) override;

    void InitDashEffect();

private:
    bool mIsDashing;
    float mDashSpeed;
    float mDashDuration;
    float mDashCooldown;
    float mDashTimer;
    float mDashCooldownTimer;
    bool mHasDashedInAir;

    DashEffectActor* mDashEffect;
};
