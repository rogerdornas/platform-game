//
// Created by roger on 26/04/2025.
//

#pragma once
#include "Actor.h"

class Sword : public Actor
{
public:
    Sword(class Game* game, Actor* owner, float width = 60.0f, float height = 20.0f, float duration = 0.1f, float damage = 10);

    void OnUpdate(float deltaTime) override;

    float GetDamage() { return mDamage; }

private:
    void Deactivate();
    void Activate();

    float mWidth;
    float mHeight;
    float mDuration;
    float mDurationTimer;
    float mDamage;

    Actor* mOwner;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
