//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

class Ground : public Actor
{
public:
    Ground(Game* game, float width, float height, bool isSpine = false);
    ~Ground();

    bool GetIsSpine() { return mIsSpine; }

private:
    float mHeight;
    float mWidth;
    bool mIsSpine;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};

