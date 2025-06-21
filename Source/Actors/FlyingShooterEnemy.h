//
// Created by roger on 09/06/2025.
//

#pragma once

#include "Actor.h"
#include "FlyingEnemySimple.h"

class FlyingShooterEnemy : public Enemy
{
public:
    enum class State
    {
        Fly,
        Shoot
    };

    FlyingShooterEnemy(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    void Fly(float deltaTime);
    void Shoot(float deltaTime);

    float mDistToSpotPlayer;
    float mFlyingAroundTimer;
    float mFlyingAroundDuration;
    float mFlyingAroundMoveSpeed;

    State mState;
    float mPatrolRangeX;             // largura da patrulha horizontal
    float mPatrolRangeY;
    float mHoverHeight;              // altura constante acima do player
    Vector2 mCurrentTarget;          // alvo atual de patrulha
    bool mTargetSet;                 // indica se já temos um alvo lateral
    float mPatrolTargetDuration;
    float mPatrolTargetTimer;

    float mFlyDuration;
    float mFlyTimer;

    float mShootDuration;
    float mShootTimer;
    bool mShot;
    float mProjectileWidth;
    float mProjectileHeight;
    float mProjectileSpeed;
};
