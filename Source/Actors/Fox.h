//
// Created by roger on 15/05/2025.
//
#pragma once

#include "Actor.h"
#include "Enemy.h"

class Fox : public Enemy
{
public:
    enum class State
    {
        Dash,
        RunAway,
        Stop,
        RunAndSword,
        Fireball,
        Jump
    };

    Fox(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;
    void SetUnlockGroundsIds(const std::vector<int>& ids) { mUnlockGroundsIds = ids; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void ResolvePlayerCollision();
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void Dash(float deltaTime);
    void RunAway(float deltaTime);
    void Stop(float deltaTime);
    void RunAndSword(float deltaTime);
    void Fireball(float deltaTime);
    void Jump(float deltaTime);


    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;

    bool mIsRunning;

    float mStopDuration;
    float mStopTimer;

    float mDashDuration;
    float mDashTimer;
    int mMaxDashes;
    int mDashCount;

    float mRunAwayDuration;
    float mRunAwayTimer;

    float mFireballDuration;
    float mFireballTimer;
    bool mAlreadyFireballed;
    float mFireballWidth;
    float mFireBallHeight;
    float mFireballSpeed;

    bool mIsOnGround;
    int mMaxJumps;
    int mJumpCount;
    float mJumpForce;
    float mGravity;

    class Sword* mSword;
    bool mSwordHitPlayer;
    float mDistToSword;

    std::vector<int> mUnlockGroundsIds;

    State mState;

    class DashComponent* mDashComponent;
};
