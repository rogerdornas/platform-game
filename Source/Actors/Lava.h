//
// Created by roger on 01/09/2025.
//

#pragma once

#include "Actor.h"
class Lava : public Actor
{
public:
    Lava(class Game* game, float width, float height, bool isMoving = false, float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);

    void OnUpdate(float deltaTime) override;

    void SetIsMoving(bool isMoving);

    void SetRespawPosition(Vector2 pos) { mRespawnPosition = pos; }
    Vector2 GetRespawPosition() { return mRespawnPosition; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void ResolveEnemyCollision();
    void ResolvePlayerCollision();
    void ResolveSwordCollision();

    float mHeight;
    float mWidth;
    float mDamage;
    Vector2 mRespawnPosition;
    bool mIsMoving;
    float mMovingDuration;
    float mMovingTimer;
    Vector2 mVelocity;

    bool mSwordHitLava;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;
    class RigidBodyComponent *mRigidBodyComponent;
    class ColliderComponent *mAABBComponent;
};
