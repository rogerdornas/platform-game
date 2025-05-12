//
// Created by roger on 02/05/2025.
//

#pragma once
#include "Actor.h"
#include "Enemy.h"

class EnemySimple : public Enemy
{
public:
    EnemySimple(Game* game, float width, float height, float movespeed, float healthpoints);
    void OnUpdate(float deltaTime) override;

private:
    void ResolveGroundCollision();
    void MovementAfterPlayerSpotted();
    void MovementBeforePlayerSpotted();

    bool mPlayerSpotted;
    float mDistToSpotPlayer;
    float mWalkingAroundTimer;
    float mWalkingAroundDuration;
    float mWalkingAroundMooveSpeed;
};
