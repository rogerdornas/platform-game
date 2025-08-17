//
// Created by roger on 16/08/2025.
//

#pragma once

#include "Component.h"
#include "ColliderComponent.h"
#include "AABBComponent.h"
#include "../Math.h"
#include <vector>

class OBBComponent : public ColliderComponent
{
public:
    OBBComponent(class Actor* owner, Vector2 halfSize);

    void Update(float deltaTime) override;

    void SetHalfSize(Vector2 halfSize) { mHalfSize = halfSize; }
    std::vector<Vector2> GetVertices();
    Vector2* GetAxis() { return mAxis; }

    bool Intersect(ColliderComponent& other) override;
    bool IntersectWithAABB(class AABBComponent& other);
    bool IntersectWithOBB(OBBComponent& other);

    Vector2 ResolveCollision(ColliderComponent &other) override;
    Vector2 ResolveCollisionWithAABB(class AABBComponent& other);
    Vector2 ResolveCollisionWithOBB(OBBComponent& other);

    Vector2 CollisionSide(ColliderComponent &other) override;
    Vector2 CollisionSideWithAABB(AABBComponent &other);
    Vector2 CollisionSideWithOBB(OBBComponent &other);

private:
    Vector2 mHalfSize;    // metade da largura e altura
    Vector2 mAxis[2];     // eixos já rotacionados
};

