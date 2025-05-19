//
// Created by roger on 02/05/2025.
//

#pragma once
#include "Actor.h"

class Enemy : public Actor
{
public:
    Enemy(Game* game, float width, float height, float movespeed, float healthPoints, float contactDamage);
    ~Enemy();

    void SetHealthPoints(float hp) { mHealthPoints = hp; }
    float GetHealthPoints() { return  mHealthPoints; }
    void ReceiveHit(float damage, Vector2 knockBackDirection);
    float GetContactDamage() { return mContactDamage; }

    float GetKnockBack() { return mKnockBackSpeed; }

protected:
    bool Died();

    float mHeight;
    float mWidth;

    float mMoveSpeed;

    float mHealthPoints;
    float mContactDamage;

    float mKnockBackSpeed;
    float mKnockBackTimer;
    float mKnockBackDuration;

    bool mIsFlashing;
    float mFlashDuration;
    float mFlashTimer;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
