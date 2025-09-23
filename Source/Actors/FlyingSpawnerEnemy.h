//
// Created by roger on 21/09/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class FlyingSpawnerEnemy : public Enemy
{
public:
    enum class State {
        Stop,
        Fly,
        FlyAway,
        SmashAttackCharge,
        SmashAttack,
        SmashAttackRecovery,
        SpawnBat
    };

    FlyingSpawnerEnemy(Game* game);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    void Stop(float deltaTime);
    void Fly(float deltaTime);
    void FlyAway(float deltaTime);
    void SmashAttackCharge(float deltaTime);
    void SmashAttack(float deltaTime);
    void SmashAttackRecovery(float deltaTime);
    void SpawnBat(float deltaTime);

    float mDistToSpotPlayer;
    float mFlyingAroundTimer;
    float mFlyingAroundDuration;
    float mFlyingAroundMoveSpeed;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;

    State mEnemyState;
    float mPatrolRangeX;             // largura da patrulha horizontal
    float mPatrolRangeY;
    float mHoverHeight;              // altura constante acima do player
    Vector2 mCurrentTarget;          // alvo atual de patrulha
    bool mTargetSet;                 // indica se já temos um alvo lateral
    float mPatrolTargetDuration;
    float mPatrolTargetTimer;

    float mFlyDuration;
    float mFlyTimer;

    float mFlyAwayDuration;
    float mFlyAwayTimer;

    float mSmashAttachChargeDuration;
    float mSmashAttackChargeTimer;
    float mSmashAttackRecoveryDuration;
    float mSmashAttackRecoveryTimer;

    float mOriginalHeight;
    float mSmashHeight;

    float mSpawnBatDuration;
    float mSpawnBatTimer;
    bool mAlreadySpawnedBat;
    int mMaxSpawnBat;
    int mCountSpawnBat;
};

