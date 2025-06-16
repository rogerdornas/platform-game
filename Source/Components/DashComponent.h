//
// Created by roger on 25/04/2025.
//

#pragma once

#include "Component.h"

class DashComponent : public Component
{
public:
    DashComponent(class Actor* owner, float dashSpeed, float dashDuration, float dashCooldown);

    void UseDash(bool isOnGround);
    bool GetIsDashing() const { return mIsDashing; }
    void SetHasDashedInAir(bool hasDashedInAir) { mHasDashedInAir = hasDashedInAir; }

    void StopDash();
    void SetDashSpeed(float dashSpeed) { mDashSpeed = dashSpeed; }
    float GetDashSpeed() const { return mDashSpeed; }

    void Update(float deltaTime) override;

private:
    bool mIsDashing;
    float mDashSpeed;
    float mDashDuration;
    float mDashCooldown;
    float mDashTimer;
    float mDashCooldownTimer;
    bool mHasDashedInAir;
};
