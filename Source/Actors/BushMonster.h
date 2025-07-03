#pragma once

#include "Actor.h"
#include "Enemy.h"

class BushMonster : public Enemy
{
public:
    enum class State {
        Idle,
        Dashing
    };

    BushMonster(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void HandleIdle(float deltaTime);
    void HandleDash(float deltaTime);

    State mBushMonsterState;
    float mGravity;

    float mIdleDuration;
    float mIdleTimer;

    float mDashDuration;
    float mDashTimer;

    float mDashSpeed;
    bool mIsDashingRight;

    float mHitDuration;
    float mHitTimer;
};
