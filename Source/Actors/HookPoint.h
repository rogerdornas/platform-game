//
// Created by roger on 28/07/2025.
//

#pragma once

#include "Actor.h"

class HookPoint : public Actor
{
public:
    enum class HookPointState {
        OutRange,
        InRange,
        Hooked
    };

    HookPoint(class Game* game);
    ~HookPoint();

    void OnUpdate(float deltaTime) override;

    float GetRadius() { return mRadius; }
    void SetHookPointState(HookPointState state) { mHookPointState = state; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mWidth;
    float mHeight;
    float mRadius;
    HookPointState mHookPointState;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;
    class ColliderComponent* mAABBComponent;
};
