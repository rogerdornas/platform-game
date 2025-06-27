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
        FlyUp
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
    void FlyUp(float deltaTime);

    void ChangeGround(float deltaTime);

    State mMothState;

    float mStopDuration;
    float mStopTimer;

    float mProjectileWidth;
    float mProjectileHeight;
    float mProjectileSpeed;
    int mMaxProjectiles;
    int mCountProjectiles;
    float mDurationBetweenProjectiles;
    float mTimerBetweenProjectiles;

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

