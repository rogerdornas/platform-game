//
// Created by roger on 19/09/2025.
//

#pragma once

#include "Actor.h"

class Spawner : public Actor
{
public:
    enum class SpawnerState {
        Appear,
        Open,
        Disappear
    };

    Spawner(class Game* game);

    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mWidth;
    float mHeight;
    float mMaxHeight;

    SpawnerState mSpawnerState;

    float mAppearDuration;
    float mAppearTimer;
    float mOpenDuration;
    float mOpenTimer;
    float mDisappearDuration;
    float mDisappearTimer;
    float mDuration;
    float mTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class ColliderComponent* mAABBComponent;

    class RectComponent* mRectComponent;
    class AnimatorComponent* mDrawComponent;
};
