//
// Created by roger on 29/04/2025.
//

#pragma once
#include "Actor.h"

class FireBall : public Actor
{
public:
    FireBall(class Game* game);
    ~FireBall();

    void OnUpdate(float deltaTime) override;

    float GetWidth() { return mWidth; }

private:
    void Deactivate();
    void Activate();

    void ResolveGroundCollision();
    void ResolveEnemyCollision();

    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mDurationTimer;

    float mDamage;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
