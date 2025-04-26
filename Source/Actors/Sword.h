//
// Created by roger on 26/04/2025.
//

#pragma once
#include "Actor.h"

class Sword : public Actor
{
public:
    Sword(class Game* game, float width = 60.0f, float height = 20.0f, float duration = 0.1f);

    void OnUpdate(float deltaTime) override;

private:
    float mWidthHorizontal;
    float mHeightHorizontal;
    float mWidthVertical;
    float mHeightVertical;
    float mTrueWidth;
    float mTrueHeight;
    float mDuration;
    float mDurationTimer;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
