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
    float mWidth;
    float mHeight;
    float mSpeed;
    float mDuration;
    float mDurationTimer;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
