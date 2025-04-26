//
// Created by roger on 22/04/2025.
//

#include "AABBComponent.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include <algorithm>
#include "../Math.h"
#include "RigidBodyComponent.h"


AABBComponent::AABBComponent(class Actor *owner, Vector2 min, Vector2 max, SDL_Color color)
    :Component(owner)
    ,mMin(min)
    ,mMax(max)
    ,mColor(color)
{
    mIsActive = true;
}


bool AABBComponent::Intersect(AABBComponent &b) {
    if (!mIsActive || !b.IsActive()) {
        return false;
    }

    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = b.GetOwner()->GetPosition();
    bool notColliding = (mMax.x + posA.x < b.mMin.x + posB.x) || (b.mMax.x + posB.x < mMin.x + posA.x) ||
                        (mMax.y + posA.y < b.mMin.y + posB.y) || (b.mMax.y + posB.y < mMin.y + posA.y);
    return !notColliding;
}


std::array<bool, 4> AABBComponent::ResolveColision(AABBComponent &b) {
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = b.GetOwner()->GetPosition();
    Vector2 vel = Vector2::Zero;
    if (GetOwner()->GetComponent<RigidBodyComponent>() != nullptr) {
        vel = GetOwner()->GetComponent<RigidBodyComponent>()->GetVelocity();
    }

    // Detecta se colidiu {top, bot, left, right}
    std::array<bool, 4> colision = {false, false, false, false};

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
    colision[3] = true;
    if (dLeft.Length() < min.Length()) {
        min = dLeft;
        colision[0] = false;
        colision[1] = false;
        colision[2] = true;
        colision[3] = false;
    }
    if (dBot.Length() < min.Length()) {
        min = dBot;
        colision[0] = false;
        colision[1] = true;
        colision[2] = false;
        colision[3] = false;
    }
    if (dTop.Length() < min.Length()) {
        min = dTop;
        colision[0] = true;
        colision[1] = false;
        colision[2] = false;
        colision[3] = false;
    }

    // parametro para colisoes em quinas
    float dist = 15;

    // Se menor distancia de colisao for top
    if (colision[0]) {
        if (Math::Abs(top - left) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            colision[0] = false;
            return colision;
        }
        if (Math::Abs(top - right) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            colision[0] = false;
            return colision;
        }
        if (vel.y >= 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + min);
            return colision;
        }
        GetOwner()->SetPosition(posA + min);
        return colision;
    }

    // Se menor distancia de colisao for bot
    if (colision[1]) {
        if (Math::Abs(bottom - left) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dLeft);
            colision[0] = false;
            colision[1] = false;
            colision[2] = true;
            colision[3] = false;
            return colision;
        }
        if (Math::Abs(bottom - right) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dRight);
            colision[0] = false;
            colision[1] = false;
            colision[2] = false;
            colision[3] = true;
            return colision;
        }
        GetOwner()->SetPosition(posA + min);
        return colision;
    }

    // Se menor distancia de colisao for left
    if (colision[2]) {
        if (Math::Abs(left - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            return colision;
        }
        if (Math::Abs(left - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            return colision;
        }
        if (Math::Abs(left - top) < dist && vel.y >= 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            colision[0] = true;
            colision[1] = false;
            colision[2] = false;
            colision[3] = false;
            return colision;
        }
        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        return colision;
    }

    // Se menor distancia de colisao for right
    if (colision[3]) {
        if (Math::Abs(right - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            return colision;
        }
        if (Math::Abs(right - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            return colision;
        }
        if (Math::Abs(right - top) < dist && vel.y >= 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            colision[0] = true;
            colision[1] = false;
            colision[2] = false;
            colision[3] = false;
            return colision;
        }
        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        return colision;
    }
    return colision;
}
