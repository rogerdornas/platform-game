//
// Created by roger on 22/04/2025.
//

#include "AABBComponent.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Math.h"
#include "RigidBodyComponent.h"


AABBComponent::AABBComponent(class Actor* owner, Vector2 min, Vector2 max)
    :Component(owner)
    ,mMin(min)
    ,mMax(max)
    ,mIsActive(true)
{
}

bool AABBComponent::Intersect(AABBComponent& b)
{
    if (!mIsActive || !b.IsActive()) {
        return false;
    }

    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = b.GetOwner()->GetPosition();
    bool notColliding = (mMax.x + posA.x < b.mMin.x + posB.x) || (b.mMax.x + posB.x < mMin.x + posA.x) ||
                        (mMax.y + posA.y < b.mMin.y + posB.y) || (b.mMax.y + posB.y < mMin.y + posA.y);
    return !notColliding;
}

std::array<bool, 4> AABBComponent::CollisionSide(AABBComponent& b) {
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = b.GetOwner()->GetPosition();

    // Detecta se colidiu {top, bottom, left, right}
    std::array<bool, 4> collision = {false, false, false, false};

    Vector2 aMin = mMin + posA;
    Vector2 aMax = mMax + posA;
    Vector2 bMin = b.mMin + posB;
    Vector2 bMax = b.mMax + posB;

    float top = bMin.y - aMax.y;
    float bottom = bMax.y - aMin.y;
    float left = bMin.x - aMax.x;
    float right = bMax.x - aMin.x;

    Vector2 dTop(0, top);
    Vector2 dBot(0, bottom);
    Vector2 dLeft(left, 0);
    Vector2 dRight(right, 0);

    Vector2 min = dRight;
    collision[3] = true;
    if (dLeft.Length() < min.Length()) {
        min = dLeft;
        collision = {false, false, true, false};
    }

    if (dBot.Length() < min.Length()) {
        min = dBot;
        collision = {false, true, false, false};
    }

    if (dTop.Length() < min.Length()) {
        min = dTop;
        collision = {true, false, false, false};
    }

    return collision;
}

std::array<bool, 4> AABBComponent::ResolveCollision(AABBComponent& b)
{
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = b.GetOwner()->GetPosition();
    Vector2 vel = Vector2::Zero;

    if (GetOwner()->GetComponent<RigidBodyComponent>() != nullptr) {
        vel = GetOwner()->GetComponent<RigidBodyComponent>()->GetVelocity();
    }

    // Detecta se colidiu {top, bottom, left, right}
    std::array<bool, 4> collision = {false, false, false, false};

    Vector2 aMin = mMin + posA;
    Vector2 aMax = mMax + posA;
    Vector2 bMin = b.mMin + posB;
    Vector2 bMax = b.mMax + posB;

    float top = bMin.y - aMax.y;
    float bottom = bMax.y - aMin.y;
    float left = bMin.x - aMax.x;
    float right = bMax.x - aMin.x;

    Vector2 dTop(0, top);
    Vector2 dBot(0, bottom);
    Vector2 dLeft(left, 0);
    Vector2 dRight(right, 0);

    Vector2 min = dRight;
    collision[3] = true;
    if (dLeft.Length() < min.Length()) {
        min = dLeft;
        collision = {false, false, true, false};
    }

    if (dBot.Length() < min.Length()) {
        min = dBot;
        collision = {false, true, false, false};
    }

    if (dTop.Length() < min.Length()) {
        min = dTop;
        collision = {true, false, false, false};
    }

    // float dist = 15;
    float dist = 1440.0f / static_cast<float>(GetGame()->GetFPS()); // conta torta
    if (dist > 25.0f) {
        dist = 25.0f;
    }

    dist *= GetGame()->GetScale();

    // Se menor distância de colisão for ‘top’
    if (collision[0]) {
        if (Math::Abs(top - left) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            collision[0] = false;
            return collision;
        }

        if (Math::Abs(top - right) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            collision[0] = false;
            return collision;
        }

        if (vel.y >= 0) {
            // GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, vel.y));
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + min);
            return collision;
        }

        GetOwner()->SetPosition(posA + min);
        return collision;
    }

    // Se menor distância de colisão for ‘bottom’
    if (collision[1]) {
        if (Math::Abs(bottom - left) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dLeft);
            collision = {false, false, true, false};
            return collision;
        }

        if (Math::Abs(bottom - right) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dRight);
            collision = {false, false, false, true};
            return collision;
        }

        if (Math::Abs(bottom - left) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + dLeft);
            collision = {false, false, true, false};
            return collision;
        }

        if (Math::Abs(bottom - right) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + dRight);
            collision = {false, false, false, true};
            return collision;
        }

        GetOwner()->SetPosition(posA + min);
        return collision;
    }

    // Se menor distância de colisão for 'left'
    if (collision[2]) {
        if (Math::Abs(left - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            return collision;
        }

        if (Math::Abs(left - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            return collision;
        }

        if (Math::Abs(left - top) < dist && vel.y > 0 && vel.x == 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            return collision;
        }

        if (Math::Abs(left - top) < dist && vel.y > 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            return collision;
        }

        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        return collision;
    }

    // Se menor distância de colisão for 'right'
    if (collision[3]) {
        if (Math::Abs(right - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            return collision;
        }

        if (Math::Abs(right - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            return collision;
        }

        if (Math::Abs(right - top) < dist && vel.y > 0 && vel.x == 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            return collision;
        }

        if (Math::Abs(right - top) < dist && vel.y > 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            return collision;
        }

        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        return collision;
    }
    return collision;
}
