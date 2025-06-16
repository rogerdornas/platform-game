//
// Created by roger on 22/04/2025.
//

#pragma once

#include "Component.h"
#include "../Math.h"
#include <array>

class AABBComponent : public Component
{
public:
    AABBComponent(class Actor* owner, Vector2 min, Vector2 max);

    void SetMin(Vector2 min) { mMin = min; }
    void SetMax(Vector2 max) { mMax = max; }
    Vector2 GetMin() { return mMin; }
    Vector2 GetMax() { return mMax; }

    void SetActive(bool active) { mIsActive = active; }
    bool IsActive() const { return mIsActive; }

    bool Intersect(AABBComponent& b);
    std::array<bool, 4> ResolveCollision(AABBComponent& b);
    std::array<bool, 4> CollisionSide(AABBComponent& b);

private:
    Vector2 mMin;
    Vector2 mMax;
    bool mIsActive;
};
