//
// Created by roger on 06/08/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class DragonFly : public Enemy
{
public:
    enum class State
    {
        FlyingAround,
        Stop,
        FollowPlayer,
        AttackStraight,
        Dive,
        Curve,
        Stum,
        Attack
    };

    DragonFly(Game* game);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    void FlyingAround(float deltaTime);
    void Stop(float deltaTime);
    void FollowPlayer(float deltaTime);
    void AttackStraight(float deltaTime);
    void Dive(float deltaTime);
    void Curve(float deltaTime);
    void Stum(float deltaTime);
    void Attack(float deltaTime);

    State mDragonFlyState;

    float mDistToSpotPlayer;
    float mFlyingAroundDuration;
    float mFlyingAroundTimer;
    float mFlyingAroundMoveSpeed;

    float mStopDuration;
    float mStopTimer;
    bool mIsCharging;

    float mHitDuration;
    float mHitTimer;

    float mAttackStraightDuration;
    float mAttackStraightTimer;
    Vector2 mAttackStraightTarget;
    float mDistToGoStraight;
    float mDistToDive;

    float mDiveDuration;
    float mDiveTimer;
    float mDiveSpeedX;
    float mDiveSpeedY;

    float mCurveDuration;
    float mCurveTimer;
    float mCurveSpeedX;
    float mCurveSpeedY;
    bool mIsCurvingRight;

    float mStumDuration;
    float mStumTimer;
    bool mStunnedAnimation;

    float mAttackDuration;
    float mAttackTimer;
};

