//
// Created by roger on 28/06/2025.
//
#pragma once

#include "Actor.h"
#include "Enemy.h"

class Golem : public Enemy
{
public:
    enum class State
    {
        Stop,
        RunAway,
        RunForward,
        Punch,
        Fireball
    };

    Golem(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ReceiveHit(float damage, Vector2 knockBackDirection) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void Stop(float deltaTime);
    void RunAway(float deltaTime);
    void RunForward(float deltaTime);
    void Punch(float deltaTime);
    void Fireball(float deltaTime);

    State mGolemState;

    bool mIsRunning;
    float mGravity;
    bool mIsInvulnerable;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;
    float mHitTimer;

    float mRunAwayDuration;
    float mRunAwayTimer;

    float mPunchDuration;
    float mPunchTimer;
    bool mPunchAnimation;
    float mDistToPunch;
    float mIdleWidth;
    float mPunchSpriteWidth;
    float mPunchOffsetHitBox;

    float mFireballDuration;
    float mFireballTimer;
    bool mAlreadyFireBalled;
    float mFireballWidth;
    float mFireBallHeight;
    float mFireballSpeed;



};


