//
// Created by roger on 03/06/2025.
//

#pragma once

#include "Actor.h"
#include "Frog.h"

class FrogTongue : public Actor
{
public:
    FrogTongue(class Game *game, Frog* owner, float damage = 20.0f);

    void OnUpdate(float deltaTime) override;

    void SetDuration(float duration);
    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolvePlayerCollision();

    void Deactivate();
    void Activate();

    float mWidth;
    float mHeight;
    float mDuration;
    float mDamage;
    int mNumOfFrames;
    float mFrameDuration;
    float mFrameTimer;
    int mFrameIndex;
    float mTongueRange;
    float mGrowSpeed;
    float mIsIncreasing;

    Frog *mOwner;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
