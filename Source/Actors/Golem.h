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
    void SetArenaMinPos(Vector2 pos) { mArenaMinPos = pos; }
    void SetArenaMaxPos(Vector2 pos) { mArenaMaxPos = pos; }

    void SetIsVulnerable() { mIsInvulnerable = false; }

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

    void ControlSpawCrystal();
    void SpawCrystal(Vector2 position);

    State mGolemState;

    bool mIsRunning;
    float mGravity;
    float mPunchProbability;

    bool mIsInvulnerable;
    bool mAlreadySpawnedCrystal;
    Vector2 mArenaMinPos;
    Vector2 mArenaMaxPos;
    float mCrystalWidth;
    float mCrystalHeight;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;
    float mHitTimer;

    float mRunAwayDuration;
    float mRunAwayTimer;
    float mMoveSpeedIncrease;

    float mPunchDuration;
    float mPunchTimer;
    float mDistToPunch;
    float mIdleWidth;
    float mPunchSpriteWidth;
    float mPunchOffsetHitBox;
    bool mPunchDirectionRight;

    float mFireballDuration;
    float mFireballTimer;
    bool mAlreadyFireBalled;
    float mFireballWidth;
    float mFireBallHeight;
    float mFireballSpeed;
    float mFireballDamage;

    // Atributos para sounds
    float mRunningSoundIntervalDuration;
    float mRunningSoundIntervalTimer;
};


