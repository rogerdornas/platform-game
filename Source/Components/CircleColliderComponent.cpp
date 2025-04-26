//
// Created by Lucas N. Ferreira on 10/09/23.
//

#include "CircleColliderComponent.h"
#include "../Actors/Actor.h"

CircleColliderComponent::CircleColliderComponent(class Actor* owner, const float radius, const int updateOrder)
    :Component(owner, updateOrder)
    ,mRadius(radius) {
}

const Vector2& CircleColliderComponent::GetCenter() const
{
    return mOwner->GetPosition();
}

float CircleColliderComponent::GetRadius() const
{
    return mOwner->GetScale() * mRadius;
}

bool CircleColliderComponent::Intersect(const CircleColliderComponent& c) const
{
    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 1.1 (2 linhas): calcule a distancia quadrada entre o centro desse círculo (GetCenter())
    //  e o do círculo c (c.GetCenter()). Primeiro, subtraia o centro do primeiro pelo do segundo e
    //  armazene o resultado em um vetor diff. Depois use o método diff.LengthSq() para calcular a distância
    //  quadrada entre os centros e armazene o resultado em um escalar distSq.
    Vector2 diff = this->GetCenter() - c.GetCenter();
    float distSq = diff.LengthSq();

    // TODO 1.2 (2 linhas): calcule o quadrado das somas dos raios e armazene o resultado em um escalar radiiSq.
    float radiiSq = pow(this->GetRadius() + c.GetRadius(), 2);

    // TODO 1.3 (1 linha): retorne verdadeiro se distSq é menor ou igual a radiiSq ou falso caso contrário.
    if (distSq <= radiiSq) {
        return true;
    }
    else {
        return false;
    }
}