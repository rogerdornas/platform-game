//
// Created by roger on 02/05/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class EnemySimple : public Enemy
{
public:
    EnemySimple(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted();
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    float mDistToSpotPlayer;
    float mPatrolRadius;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMoveSpeed;
    float mGravity;
};
