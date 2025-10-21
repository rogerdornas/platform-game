//
// Created by roger on 29/04/2025.
//

#pragma once

#include "Actor.h"
#include "../AudioSystem.h"

class FireBall : public Actor
{
public:
    enum class State
    {
        Deactivate,
        Throwing,
        Exploding
    };

    FireBall(class Game *game);
    ~FireBall() override;

    void OnUpdate(float deltaTime) override;

    float GetWidth() override { return mWidth; }
    void SetWidth(float w) { mWidth = w; mThrowingWidth = w; mExplodingWidth = w * 1.3f; }
    void SetHeight(float h) { mHeight = h; mThrowingWidth = h; mExplodingWidth = h * 1.3f; }
    void SetSpeed(float s) { mSpeed = s; }
    float GetSpeed() const { return mSpeed; }
    void SetDamage(float damage) { mDamage = damage; }
    float GetDamage() const { return mDamage; }
    void SetIsFromEnemy() { mIsFromEnemy = true; }
    bool GetIsFromEnemy() const { return mIsFromEnemy; }
    void ExplodeFireball() { mDurationTimer = mDuration; }

    void Activate();
    void Deactivate();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveGroundCollision();
    void ResolveEnemyCollision();
    void ResolvePlayerCollision();

    void ExplosionEffect();
    bool IsOnScreen();

    void ManageAnimations();

    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mDurationTimer;
    float mThrowingWidth;
    float mExplodingWidth;

    State mFireballState;
    float mFireballOffscreenLimit;
    float mDeactivateDuration;
    float mDeactivateTimer;

    float mDamage;
    bool mIsFromEnemy;

    SoundHandle mSound;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class ColliderComponent *mAABBComponent;
};
