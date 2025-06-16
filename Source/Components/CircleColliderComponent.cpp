//
// Created by Lucas N. Ferreira on 10/09/23.
//

#include "CircleColliderComponent.h"
#include "../Actors/Actor.h"

CircleColliderComponent::CircleColliderComponent(class Actor* owner, const float radius, const int updateOrder)
    :Component(owner, updateOrder)
    ,mRadius(radius)
{
}

const Vector2 &CircleColliderComponent::GetCenter() const {
    return mOwner->GetPosition();
}

float CircleColliderComponent::GetRadius() const {
    return mOwner->GetScale() * mRadius;
}

bool CircleColliderComponent::Intersect(const CircleColliderComponent &c) const {
    Vector2 diff = this->GetCenter() - c.GetCenter();
    float distSq = diff.LengthSq();

    float radiiSq = pow(this->GetRadius() + c.GetRadius(), 2);

    return distSq <= radiiSq;
}
