//
// Created by roger on 03/06/2025.
//

#pragma once
#include "Actor.h"

class FrogTongue : public Actor
{
public:
    FrogTongue(class Game *game, Actor *owner, float damage = 20.0f);

    void OnUpdate(float deltaTime) override;

    float GetDamage() { return mDamage; }
    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }
    void SetDuration(float duration);

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolvePlayerCollision();

    void Deactivate();
    void Activate();

    void Increase();
    void Decrease();

    float mWidth;
    float mHeight;
    float mDuration;
    float mDurationTimer;
    float mIncreaseDuration;
    float mIncreaseTimer;
    float mDecreaseDuration;
    float mDecreaseTimer;
    float mDamage;
    int mNumOfFrames;
    float mFrameDuration;
    float mFrameTimer;
    int mFrameIndex;
    float mGrowDist;
    float mGrowSpeed;
    float mIsIncreasing;
    int mMaxAttack;
    int mAttackCount;

    Actor *mOwner;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
