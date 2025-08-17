//
// Created by roger on 22/04/2025.
//

#pragma once

#include "Component.h"
#include "ColliderComponent.h"
#include "OBBComponent.h"
#include "../Math.h"
#include <array>

class AABBComponent : public ColliderComponent
{
public:
    AABBComponent(class Actor* owner, Vector2 min, Vector2 max);

    void SetMin(Vector2 min) { mMin = min; }
    void SetMax(Vector2 max) { mMax = max; }
    Vector2 GetMin() { return mMin; }
    Vector2 GetMax() { return mMax; }

    bool Intersect(ColliderComponent& other) override;
    bool IntersectWithAABB(AABBComponent& other);
    bool IntersectWithOBB(class OBBComponent& other);

    Vector2 ResolveCollision(ColliderComponent &other) override;
    Vector2 ResolveCollisionWithAABB(AABBComponent& other);
    Vector2 ResolveCollisionWithOBB(class OBBComponent& other);

    Vector2 CollisionSide(ColliderComponent &other) override;
    Vector2 CollisionSideWithAABB(AABBComponent &other);
    Vector2 CollisionSideWithOBB(OBBComponent &other);

private:
    Vector2 mMin;
    Vector2 mMax;
};
