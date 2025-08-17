//
// Created by roger on 22/04/2025.
//

#include "AABBComponent.h"
#include <cfloat>
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Math.h"
#include "RigidBodyComponent.h"

AABBComponent::AABBComponent(class Actor* owner, Vector2 min, Vector2 max)
    :ColliderComponent(owner, ColliderType::AABB)
    ,mMin(min)
    ,mMax(max)
{
}

bool AABBComponent::Intersect(ColliderComponent& other) {
    if (!mIsActive || !other.IsActive()) {
        return false;
    }

    switch (other.GetType()) {
        case ColliderType::AABB:
            return IntersectWithAABB(*dynamic_cast<AABBComponent*>(&other));
            break;
        case ColliderType::OBB:
            return IntersectWithOBB(*dynamic_cast<OBBComponent*>(&other));
            break;
    }
    return false;
}

bool AABBComponent::IntersectWithAABB(AABBComponent& other) {
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = other.GetOwner()->GetPosition();
    bool notColliding = (mMax.x + posA.x < other.mMin.x + posB.x) || (other.mMax.x + posB.x < mMin.x + posA.x) ||
                        (mMax.y + posA.y < other.mMin.y + posB.y) || (other.mMax.y + posB.y < mMin.y + posA.y);
    return !notColliding;
}

bool AABBComponent::IntersectWithOBB(OBBComponent &other) {
    // Obtém vértices do AABB
    Vector2 AABBPosition = GetOwner()->GetPosition();
    std::vector<Vector2> vertsAABB = {
        Vector2(mMin.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMax.y + AABBPosition.y),
        Vector2(mMin.x + AABBPosition.x, mMax.y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = other.GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += other.GetOwner()->GetPosition();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // eixo X global (AABB)
        Vector2(0, 1),      // eixo Y global (AABB)
        other.GetAxis()[0],         // eixo X do OBB
        other.GetAxis()[1]          // eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsAABB, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return false; // separação → sem colisão
        }
    }

    return true; // colidiu
}

Vector2 AABBComponent::ResolveCollision(ColliderComponent &other) {
    switch (other.GetType()) {
        case ColliderType::AABB:
            return ResolveCollisionWithAABB(*dynamic_cast<AABBComponent*>(&other));
            break;
        case ColliderType::OBB:
            return ResolveCollisionWithOBB(*dynamic_cast<OBBComponent*>(&other));
            break;
    }
    return Vector2::Zero;
}

Vector2 AABBComponent::ResolveCollisionWithAABB(AABBComponent &other) {
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = other.GetOwner()->GetPosition();
    Vector2 vel = Vector2::Zero;

    if (GetOwner()->GetComponent<RigidBodyComponent>() != nullptr) {
        vel = GetOwner()->GetComponent<RigidBodyComponent>()->GetVelocity();
    }

    // Detecta se colidiu {top, bottom, left, right}
    std::array<bool, 4> collision = {false, false, false, false};
    Vector2 normal(Vector2::Zero);

    Vector2 aMin = mMin + posA;
    Vector2 aMax = mMax + posA;
    Vector2 bMin = other.mMin + posB;
    Vector2 bMax = other.mMax + posB;

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
            // return collision;
            normal = Vector2::Zero;
            return normal;
        }

        if (Math::Abs(top - right) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            collision[0] = false;
            // return collision;
            normal = Vector2::Zero;
            return normal;
        }

        if (vel.y >= 0) {
            // GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, vel.y));
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + min);
            // return collision;
            normal = Vector2::NegUnitY;
            return normal;
        }

        GetOwner()->SetPosition(posA + min);
        // return collision;
        normal = Vector2::NegUnitY;
        return normal;
    }

    // Se menor distância de colisão for ‘bottom’
    if (collision[1]) {
        if (Math::Abs(bottom - left) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dLeft);
            collision = {false, false, true, false};
            // return collision;
            normal = Vector2::NegUnitX;
            return normal;
        }

        if (Math::Abs(bottom - right) < dist && vel.y > 0) {
            GetOwner()->SetPosition(posA + dRight);
            collision = {false, false, false, true};
            // return collision;
            normal = Vector2::UnitX;
            return normal;
        }

        if (Math::Abs(bottom - left) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + dLeft);
            collision = {false, false, true, false};
            // return collision;
            normal = Vector2::NegUnitX;
            return normal;
        }

        if (Math::Abs(bottom - right) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + dRight);
            collision = {false, false, false, true};
            // return collision;
            normal = Vector2::UnitX;
            return normal;
        }

        GetOwner()->SetPosition(posA + min);
        // return collision;
        normal = Vector2::UnitY;
        return normal;
    }

    // Se menor distância de colisão for 'left'
    if (collision[2]) {
        if (Math::Abs(left - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            // return collision;
            normal = Vector2::NegUnitX;
            return normal;
        }

        if (Math::Abs(left - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            // return collision;
            normal = Vector2::NegUnitX;
            return normal;
        }

        if (Math::Abs(left - top) < dist && vel.y > 0 && vel.x == 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            // return collision;
            normal = Vector2::NegUnitY;
            return normal;
        }

        if (Math::Abs(left - top) < dist && vel.y > 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            // return collision;
            normal = Vector2::NegUnitY;
            return normal;
        }

        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        // return collision;
        normal = Vector2::NegUnitX;
        return normal;
    }

    // Se menor distância de colisão for 'right'
    if (collision[3]) {
        if (Math::Abs(right - bottom) < dist) {
            GetOwner()->SetPosition(posA + min);
            // return collision;
            normal = Vector2::UnitX;
            return normal;
        }

        if (Math::Abs(right - top) < dist && vel.y < 0) {
            GetOwner()->SetPosition(posA + min);
            // return collision;
            normal = Vector2::UnitX;
            return normal;
        }

        if (Math::Abs(right - top) < dist && vel.y > 0 && vel.x == 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            // return collision;
            normal = Vector2::NegUnitY;
            return normal;
        }

        if (Math::Abs(right - top) < dist && vel.y > 0) {
            GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(vel.x, 0));
            GetOwner()->SetPosition(posA + dTop);
            collision = {true, false, false, false};
            // return collision;
            normal = Vector2::NegUnitY;
            return normal;
        }

        GetOwner()->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, vel.y));
        GetOwner()->SetPosition(posA + min);
        // return collision;
        normal = Vector2::UnitX;
        return normal;
    }
    // return collision;
    normal = Vector2::Zero;
    return normal;
}

Vector2 AABBComponent::ResolveCollisionWithOBB(OBBComponent &other) {
    // Obtém vértices do AABB
    Vector2 AABBPosition = GetOwner()->GetPosition();
    std::vector<Vector2> vertsAABB = {
        Vector2(mMin.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMax.y + AABBPosition.y),
        Vector2(mMin.x + AABBPosition.x, mMax.y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = other.GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += other.GetOwner()->GetPosition();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // eixo X global (AABB)
        Vector2(0, 1),      // eixo Y global (AABB)
        other.GetAxis()[0],         // eixo X do OBB
        other.GetAxis()[1]          // eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float penetration = FLT_MAX;
    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsAABB, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // não colidiu
        }

        // Calcula quanto penetrou neste eixo
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < penetration) {
            penetration = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal
    Vector2 centerA = GetOwner()->GetPosition();
    Vector2 dir = other.GetOwner()->GetPosition() - centerA;
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    // Vetor de correção
    Vector2 correction = normal * penetration;

    // Move o AABB para fora do OBB
    GetOwner()->SetPosition(centerA + correction);

    return normal;
}

Vector2 AABBComponent::CollisionSide(ColliderComponent &other) {
    switch (other.GetType()) {
        case ColliderType::AABB:
            return CollisionSideWithAABB(*dynamic_cast<AABBComponent*>(&other));
        break;
        case ColliderType::OBB:
            return CollisionSideWithOBB(*dynamic_cast<OBBComponent*>(&other));
        break;
    }
    return Vector2::Zero;
}

Vector2 AABBComponent::CollisionSideWithAABB(AABBComponent &other) {
    Vector2 posA = GetOwner()->GetPosition();
    Vector2 posB = other.GetOwner()->GetPosition();

    Vector2 normal(Vector2::Zero);

    Vector2 aMin = mMin + posA;
    Vector2 aMax = mMax + posA;
    Vector2 bMin = other.mMin + posB;
    Vector2 bMax = other.mMax + posB;

    float top = bMin.y - aMax.y;
    float bottom = bMax.y - aMin.y;
    float left = bMin.x - aMax.x;
    float right = bMax.x - aMin.x;

    Vector2 dTop(0, top);
    Vector2 dBot(0, bottom);
    Vector2 dLeft(left, 0);
    Vector2 dRight(right, 0);

    Vector2 min = dRight;
    normal = Vector2::UnitX;
    if (dLeft.Length() < min.Length()) {
        min = dLeft;
        normal = Vector2::NegUnitX;
    }

    if (dBot.Length() < min.Length()) {
        min = dBot;
        normal = Vector2::UnitY;
    }

    if (dTop.Length() < min.Length()) {
        min = dTop;
        normal = Vector2::NegUnitY;
    }
    return normal;
}

Vector2 AABBComponent::CollisionSideWithOBB(OBBComponent &other) {
    // Obtém vértices do AABB
    Vector2 AABBPosition = GetOwner()->GetPosition();
    std::vector<Vector2> vertsAABB = {
        Vector2(mMin.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMin.y + AABBPosition.y),
        Vector2(mMax.x + AABBPosition.x, mMax.y + AABBPosition.y),
        Vector2(mMin.x + AABBPosition.x, mMax.y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = other.GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += other.GetOwner()->GetPosition();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // eixo X global (AABB)
        Vector2(0, 1),      // eixo Y global (AABB)
        other.GetAxis()[0],         // eixo X do OBB
        other.GetAxis()[1]          // eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float penetration = FLT_MAX;
    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsAABB, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // não colidiu
        }

        // Calcula quanto penetrou neste eixo
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < penetration) {
            penetration = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal
    Vector2 centerA = GetOwner()->GetPosition();
    Vector2 dir = other.GetOwner()->GetPosition() - centerA;
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    return normal;
}
