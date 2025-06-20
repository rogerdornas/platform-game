//
// Created by roger on 26/04/2025.
//

#pragma once

#include "Actor.h"

class Sword : public Actor
{
public:
    Sword(class Game *game, Actor *owner, float width = 60.0f, float height = 20.0f, float duration = 0.1f, float damage = 10);

    void OnUpdate(float deltaTime) override;

    float GetDamage() const { return mDamage; }
    void SetDamage(float damage) { mDamage = damage; }
    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }
    void SetWidth(float width) { mWidth = width; }
    void SetHeight(float height) { mHeight = height; }


    void ChangeResolution(float oldScale, float newScale) override;

private:
    void Deactivate();
    void Activate();

    float mWidth;
    float mHeight;
    float mDuration;
    float mDurationTimer;
    float mDamage;

    Actor *mOwner;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
