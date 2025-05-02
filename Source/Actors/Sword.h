//
// Created by roger on 26/04/2025.
//

#pragma once
#include "Actor.h"

class Sword : public Actor
{
public:
    Sword(class Game* game, float width = 60.0f, float height = 20.0f, float duration = 0.1f, float damage = 10);

    void OnUpdate(float deltaTime) override;

    float GetDamage() { return mDamage; }

private:
    float mWidthHorizontal;
    float mHeightHorizontal;
    float mWidthVertical;
    float mHeightVertical;
    float mTrueWidth;
    float mTrueHeight;
    float mDuration;
    float mDurationTimer;
    float mDamage;

    class DrawComponent* mDrawComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
