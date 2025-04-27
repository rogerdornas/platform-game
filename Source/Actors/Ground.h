//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

class Ground : public Actor
{
public:
    Ground(Game* game, float width, float height);
    ~Ground();

private:
    float mHeight;
    float mWidth;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};

