//
// Created by roger on 15/09/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class HookEnemy : public Enemy
{
public:
    enum class State {
        Stop,
        Hook,
        DiagonalAttack,
        ForwardAttack
    };

    HookEnemy(Game* game, float width, float height, float moveSpeed, float healthPoints);

    void OnUpdate(float deltaTime) override;
    State GetHookEnemyState() const { return mHookEnemyState; }
    int GetLastForwardAttackDirection() const { return mLastForwardAttackDirection; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision() override;
    void ResolveEnemyCollision() override;
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void TriggerBossDefeat();

    void ManageAnimations();

    void Stop(float deltaTime);
    void Hook(float deltaTime);
    void DiagonalAttack(float deltaTime);
    void ForwardAttack(float deltaTime);

    State mHookEnemyState;

    float mGravity;
    bool mIsOnGround;

    float mStopDuration;
    float mStopTimer;

    float mHitDuration;

    float mHookStateDuration;
    float mHookStateTimer;
    bool mIsHooking;
    Vector2 mHookDirection;
    float mHookSpeed;
    float mHookingDuration;
    float mHookingTimer;
    Vector2 mHookEnd;
    float mHookAnimProgress;
    bool mIsHookAnimating;
    class HookPoint* mHookPoint;
    float mHookAnimationDuration; // duração do efeito visual
    int mHookSegments;
    float mHookAmplitude;
    float mHookSegmentHeight;

    float mForwardAttackDuration;
    float mForwardAttackTimer;
    float mForwardAttackDelayDuration;
    float mForwardAttackDelayTimer;
    int mLastForwardAttackDirection;

    float mSpeedIncreaseRate;

    class DrawRopeComponent* mDrawRopeComponent;
};

