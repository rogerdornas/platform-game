//
// Created by roger on 09/06/2025.
//

#pragma once
#include "Actor.h"
#include "FlyingEnemySimple.h"

class FlyingShooterEnemy : public FlyingEnemySimple
{
public:
    enum class State
    {
        Fly,
        Shoot
    };

    FlyingShooterEnemy(Game* game, float width, float height, float movespeed, float healthpoints);

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime) override;

    void Fly(float deltaTime);
    void Shoot(float deltaTime);

    FlyingShooterEnemy::State mState;
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
