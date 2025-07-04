//
// Created by roger on 02/05/2025.
//

#pragma once

#include "Actor.h"

class Enemy : public Actor
{
public:
    Enemy(Game* game, float width, float height, float moveSpeed, float healthPoints, float contactDamage);
    ~Enemy() override;

    void SetHealthPoints(float hp) { mHealthPoints = hp; }
    float GetHealthPoints() const { return mHealthPoints; }
    virtual void ReceiveHit(float damage, Vector2 knockBackDirection);
    float GetContactDamage() const { return mContactDamage; }
    float GetKnockBack() const { return mKnockBackSpeed; }
    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }
    void SetSpottedPlayer(bool spotted) { mPlayerSpotted = spotted; }

    void SetId(int id) { mId = id; }
    int GetId() const { return mId; }

protected:
    bool Died();
    void ResolveEnemyCollision() const;
    virtual void ResolveGroundCollision();
    bool IsOnScreen();

    int mId;
    float mWidth;
    float mHeight;

    int mMoneyDrop;

    float mMoveSpeed;

    float mMaxHealthPoints;
    float mHealthPoints;
    float mContactDamage;

    float mKnockBackSpeed;
    float mKnockBackTimer;
    float mKnockBackDuration;
    float mCameraShakeStrength;

    bool mIsFlashing;
    float mFlashDuration;
    float mFlashTimer;

    bool mPlayerSpotted;

    float mOffscreenLimit;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
