//
// Created by roger on 02/05/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class FlyingEnemySimple : public Enemy
{
public:
    FlyingEnemySimple(Game* game);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    float mDistToSpotPlayer;
    float mFlyingAroundDuration;
    float mFlyingAroundTimer;
    float mFlyingAroundMoveSpeed;
};
