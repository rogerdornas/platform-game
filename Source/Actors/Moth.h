//
// Created by roger on 25/06/2025.
//
#pragma once

#include "Actor.h"
#include "Enemy.h"

class Moth : public Enemy
{
public:
    enum class State
    {
        Stop,
        Projectiles,
        SlowMotionProjectiles,
        CircleProjectiles,
        BoostUp,
        FlyUp,
        Dying
    };

    Moth(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void Stop(float deltaTime);
    void Projectiles(float deltaTime);
    void SlowMotionProjectiles(float deltaTime);
    void CircleProjectiles(float deltaTime);
    void BoostUp(float deltaTime);
    void FlyUp(float deltaTime);
    void Dying(float deltaTime);

    void ChangeGround(float deltaTime);

    State mMothState;
    bool mIsSlowMotion;
    float mSlowMotionProbability;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;
    float mHitTimer;

    float mDyingDuration;
    float mDyingTimer;

    float mBoostUpDuration;
    float mBoostUpTimer;
    bool mAlreadyBoosted;

    float mAttackDuration;
    float mAttackTimer;
    bool mAttackAnimation;

    float mProjectileWidth;
    float mProjectileHeight;
    float mProjectileSpeed;
    int mMaxProjectiles;
    int mCountProjectiles;
    float mDurationBetweenProjectiles;
    float mTimerBetweenProjectiles;

    int mProjectilesStartDirection;

    float mCircleProjectileWidth;
    float mCircleProjectileHeight;
    float mCircleProjectileSpeed;
    int mMaxCircleProjectiles;
    int mCountCircleProjectiles;
    float mCircleProjectilesDuration;
    float mCircleProjectilesTimer;

    float mFlyUpDuration;
    float mFlyUpTimer;


};

