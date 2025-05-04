//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

class Ground : public Actor
{
public:
    Ground(Game* game, float width, float height, bool isSpine = false, bool isMoving = false, float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);
    ~Ground();

    void OnUpdate(float deltaTime) override;

    bool GetIsSpine() { return mIsSpine; }
    bool GetIsMoving() { return mIsMoving; }

    float GetHeight() { return mHeight; }

    float GetWidth() { return mWidth; }

private:
    float mHeight;
    float mWidth;
    bool mIsSpine;
    bool mIsMoving;
    float mMovingTimer;
    float mMovingDuration;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};

