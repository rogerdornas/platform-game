//
// Created by roger on 02/05/2025.
//

#pragma once
#include "Actor.h"
#include "Enemy.h"

class FlyingEnemySimple : public Enemy
{
public:
    FlyingEnemySimple(Game* game, float width, float height, float movespeed, float healthpoints);
    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

protected:
    void ResolveGroundCollision();
    virtual void MovementAfterPlayerSpotted(float deltaTime);
    void MovementBeforePlayerSpotted();

    float mDistToSpotPlayer;
    float mFlyingAroundTimer;
    float mFlyingAroundDuration;
    float mFlyingAroundMoveSpeed;
};
