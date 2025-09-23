//
// Created by roger on 29/07/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class FlyingGolem : public Enemy
{
public:
    enum class State
    {
        FlyingAround,
        Stop,
        FlyForward,
        Attack,
        TeleportIn,
        TeleportOut
    };

    FlyingGolem(Game* game);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    void FlyingAround(float deltaTime);
    void Stop(float deltaTime);
    void FlyForward(float deltaTime);
    void Attack(float deltaTime);
    void TeleportIn(float deltaTime);
    void TeleportOut(float deltaTime);

    State mFlyingGolemState;

    float mDistToSpotPlayer;
    float mFlyingAroundDuration;
    float mFlyingAroundTimer;
    float mFlyingAroundMoveSpeed;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;
    float mHitTimer;

    float mRunAwayDuration;
    float mRunAwayTimer;
    float mMoveSpeedIncrease;

    float mAttackDuration;
    float mAttackTimer;
    float mDistToAttack;
    float mIdleWidth;
    float mAttackSpriteWidth;
    float mAttackOffsetHitBox;
    bool mAttackDirectionRight;

    float mDistToTeleport;
    float mTeleportDuration;
    float mTeleportInTimer;
    float mTeleportOutTimer;
    Vector2 mTeleportTargetPosition;
    float mTeleportHoverHeight;
    float mTeleportRangeTargetX;
};


