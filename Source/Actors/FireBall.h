//
// Created by roger on 29/04/2025.
//

#pragma once
#include "Actor.h"

class FireBall : public Actor
{
public:
    FireBall(class Game *game);
    ~FireBall();

    void OnUpdate(float deltaTime) override;

    float GetWidth() { return mWidth; }
    void SetWidth(float w) { mWidth = w; }
    void SetHeight(float h) { mHeight = h; }
    void SetSpeed(float s) { mSpeed = s; }
    void SetIsFromEnemy() { mIsFromEnemy = true; }

    void Activate();
    void Deactivate();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolveEnemyCollision();
    void ResolvePlayerCollision();

    void ManageAnimations();

    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mDurationTimer;

    float mDamage;

    bool mIsFromEnemy;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
