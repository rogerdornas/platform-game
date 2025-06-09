//
// Created by roger on 02/06/2025.
//
#pragma once
#include "Actor.h"
#include "Enemy.h"

enum FrogState
{
    frogStop,
    frogJumpCombo,
    frogTongue
};

enum FrogWallSide
{
    frogBottom,
    frogTop,
    frogLeft,
    frogRight
};


class Frog : public Enemy
{
public:
    Frog(Game *game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;
    void SetArenaMinPos(Vector2 pos) { mArenaMinPos = pos; }
    void SetArenaMaxPos(Vector2 pos) { mArenaMaxPos = pos; }
    void SetUnlockGroundsIds(const std::vector<int>& ids) { mUnlockGroundsIds = ids; }
    void SetIsLicking(bool isLicking) { mIsLicking = isLicking; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolvePlayerCollision();
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void Stop(float deltaTime);
    void JumpCombo(float deltaTime);
    void Tongue(float delTime);


    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;

    bool mIsRunning;

    float mStopDuration;
    float mStopTimer;

    bool mIsOnGround;
    int mMaxJumps;
    int mJumpCount;
    float mJumpForce;
    float mTimerBetweenJumps;
    float mDurationBetweenJumps;

    FrogWallSide mWallPosition;
    FrogWallSide mDestinyWall;
    float mGravity;

    Vector2 mArenaMinPos;
    Vector2 mArenaMaxPos;
    float mMinDistFromEdge;
    int mAttackJumpInterval;
    class FrogTongue *mTongue;
    float mTongueDuration;
    float mTongueTimer;
    bool mIsLicking;
    float mJumpComboProbability;

    std::vector<int> mUnlockGroundsIds;

    FrogState mState;
};
