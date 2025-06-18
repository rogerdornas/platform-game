//
// Created by roger on 17/06/2025.
//

#pragma once

#include "Actor.h"

class Money : public Actor
{
public:
    enum class MoneyType {
        Small,  // 1
        Medium, // 5
        Large   // 10
    };

    Money(class Game* game, MoneyType type = MoneyType::Small);

    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    enum class State {
        FlyingOut,
        Hovering,
        Homing
    };

    int mValue;
    float mWidth;
    float mHeight;
    State mMoneyState;

    float mFlySpeed;
    float mHomingSpeed;

    float mFlyDuration;
    float mFlyTimer;
    float mHoverDuration;
    float mHoverTimer;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawParticleComponent* mDrawParticleComponent;
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
