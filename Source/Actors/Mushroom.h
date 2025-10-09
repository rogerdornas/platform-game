//
// Created by roger on 07/10/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class Mushroom : public Enemy
{
public:
    enum class State
    {
        Stop,
        WalkForward,
        WalkAway,
        Attack,
        Stun
    };

    Mushroom(Game* game);
    void OnUpdate(float deltaTime) override;
    State GetMushroomState() const { return mMushroomState; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ReceiveHit(float damage, Vector2 knockBackDirection) override;

    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted(float deltaTime);

    void Stop(float deltaTime);
    void WalkForward(float deltaTime);
    void WalkAway(float deltaTime);
    void Attack(float deltaTime);
    void Stun(float deltaTime);

    void ManageAnimations();

    State mMushroomState;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;

    float mDistToSpotPlayer;
    float mLookingAroundDuration;
    float mLookingAroundTimer;

    float mWalkAwayDuration;
    float mWalkAwayTimer;

    float mGravity;

    float mDistToAttack;
    float mJumpForce;
    float mAttackDuration;
    float mAttackTimer;
    float mAttackOffsetHitBox;

    float mStumDuration;
    float mStumTimer;
};
