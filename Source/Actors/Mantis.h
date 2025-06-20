//
// Created by roger on 20/06/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class Mantis : public Enemy
{
public:
    Mantis(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    enum class State
    {
        WalkForward,
        WalkBack,
        Attack
    };

    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void WalkForward(float deltaTime);
    void WalkBack(float deltaTime);
    void Attack(float deltaTime);

    void ManageAnimations();

    State mMantisState;
    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;
    float mGravity;
    float mDistToAttack;
    float mJumpForce;
    float mAttackDuration;
    float mAttackTimer;
    float mWalkBackDuration;
    float mWalkBackTimer;
    float mWaitToAttackDuration;
    float mWaitToAttackTimer;
};
