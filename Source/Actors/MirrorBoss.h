//
// Created by roger on 11/10/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class MirrorBoss : public Enemy
{
public:
    enum class State {
        Stop,
        Projectiles,
        InvertControl,
        SpawnEnemy,
        SpawnCloneEnemy,
        TeleportIn,
        TeleportOut
    };

    MirrorBoss(Game* game);

    void OnUpdate(float deltaTime) override;
    void SetArenaMinPos(Vector2 pos) { mArenaMinPos = pos; }
    void SetArenaMaxPos(Vector2 pos) { mArenaMaxPos = pos; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementBeforePlayerSpotted();
    void MovementAfterPlayerSpotted(float deltaTime);

    void TriggerBossDefeat();

    void ManageAnimations();

    void Stop(float deltaTime);
    void Projectiles(float deltaTime);
    void InvertControl(float deltaTime);
    void SpawnEnemy(float deltaTime);
    void SpawnCloneEnemy(float deltaTime);
    void TeleportIn(float deltaTime);
    void TeleportOut(float deltaTime);

    State mBossState;

    Vector2 mArenaMinPos;
    Vector2 mArenaMaxPos;

    float mStopDuration;
    float mStopDurationFast;
    float mStopDurationSlow;
    float mStopTimer;

    float mHitDuration;

    float mProjectileWidth;
    float mProjectileHeight;
    float mProjectileSpeed;
    float mProjectileDamage;
    float mProjectileDuration;
    float mProjectileSpread;
    int mNumProjectileBounces;
    int mMaxProjectiles;
    int mCountProjectiles;
    float mDurationBetweenProjectiles;
    float mTimerBetweenProjectiles;

    float mMirrorWidth;
    float mMirrorHeight;

    float mSpawnEnemyDuration;
    float mSpawnEnemyTimer;
    bool mAlreadySpawnedEnemy;
    float mSpawnPortalDuration;
    float mSpawnPortalTimer;
    Vector2 mSpawnPosition;
    class CloneEnemy* mCloneEnemy;
    bool mCloneEnemyDied;
    std::vector<Vector2> mSpawnPoints;

    float mTeleportDuration;
    float mTeleportDurationFast;
    float mTeleportDurationNormal;
    float mTeleportInTimer;
    float mTeleportOutTimer;
    float mDistFromPlayerToStartTeleport;
    float mDistFromPlayerToEndTeleport;
    Vector2 mTeleportTargetPosition;
};
