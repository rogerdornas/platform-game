//
// Created by roger on 21/09/2025.
//

#pragma once

#include "Actor.h"
#include "Enemy.h"

class LittleBat : public Enemy
{
    public:
    LittleBat(Game* game, float width, float height, float moveSpeed, float healthPoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    void ManageAnimations();

    float mDistToSpotPlayer;
    float mFlyingAroundTimer;
    float mFlyingAroundDuration;
    float mFlyingAroundMoveSpeed;
};

