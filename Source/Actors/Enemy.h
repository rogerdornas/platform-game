//
// Created by roger on 02/05/2025.
//

#pragma once
#include "Actor.h"

class Enemy : public Actor
{
public:
    Enemy(Game* game, float width, float height, float movespeed, float healthPoints);
    ~Enemy();

    void SetHealthPoints(float hp) { mHealthPoints = hp; }
    float GetHealthPoints() { return  mHealthPoints; }
    void ReceiveHit(float damage, Vector2 knockBackDirection);

    float GetKnockBack() { return mKnockBack; }

protected:
    bool Died();

    float mHeight;
    float mWidth;

    float mMoveSpeed;

    float mHealthPoints;

    float mKnockBack;
    float mKnockBackTimer;
    float mKnockBackDuration;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
