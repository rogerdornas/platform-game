//
// Created by roger on 07/10/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class Snake : public Enemy
{
public:
    Snake(Game* game);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    enum class State
    {
        WalkForward,
        Stop,
        Attack
    };

    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void WalkForward(float deltaTime);
    void Stop(float deltaTime);
    void Attack(float deltaTime);

    void ManageAnimations();

    State mSnakeState;

    float mStopDuration;
    float mStopTimer;

    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;

    float mGravity;

    float mDistToAttack;
    float mAttackDuration;
    float mAttackTimer;
    float mIdleWidth;
    float mAttackSpriteWidth;
    float mAttackOffsetHitBox;
};

