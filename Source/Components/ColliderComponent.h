//
// Created by roger on 16/08/2025.
//

#pragma once

#include "Component.h"
#include "../Math.h"
#include <array>
#include <vector>

class ColliderComponent : public Component
{
public:
    enum class ColliderType { AABB, OBB };

    ColliderComponent(class Actor* owner, ColliderType type);

    ColliderType GetType() const { return mType; }

    void SetActive(bool active) { mIsActive = active; }
    bool IsActive() const { return mIsActive; }

    virtual bool Intersect(ColliderComponent& other) = 0;
    virtual Vector2 ResolveCollision(ColliderComponent& other) = 0;
    virtual Vector2 CollisionSide(ColliderComponent& other) = 0;

protected:
    ColliderType mType;
    bool mIsActive;
};

