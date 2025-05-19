//
// Created by Lucas N. Ferreira on 08/09/23.
//

#include "RigidBodyComponent.h"
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"

RigidBodyComponent::RigidBodyComponent(class Actor *owner, float mass, float maxSpeedX, float maxSpeedY,
                                       int updateOrder)
    : Component(owner, updateOrder),
      mMass(mass),
      mAngularSpeed(0.0f),
      mMaxSpeedX(maxSpeedX),
      mMaxSpeedY(maxSpeedY),
      mVelocity(Vector2::Zero),
      mAcceleration(Vector2::Zero) {}

void RigidBodyComponent::ApplyForce(const Vector2 &force) { mAcceleration += force * (1.0f / mMass); }

void RigidBodyComponent::Update(float deltaTime)
{
    Vector2 position = mOwner->GetPosition();

    // --------------
    // TODO - PARTE 2
    // --------------

    // TODO 2.1 (2 linhas): Atualize a velocidade (mVelocity) e a posição (position) do objeto utilizando
    //  o método de Euler semi-implícito.
    mVelocity += mAcceleration * deltaTime;

    if (Math::Abs(mVelocity.x) > mMaxSpeedX)
    {
        if (mVelocity.x > 0)
            mVelocity.x = mMaxSpeedX;

        else
            mVelocity.x = -mMaxSpeedX;
    }

    if (Math::Abs(mVelocity.y) > mMaxSpeedY)
    {
        if (mVelocity.y > 0)
            mVelocity.y = mMaxSpeedY;

        else
            mVelocity.y = -mMaxSpeedY;
    }

    position += mVelocity * deltaTime;

    mOwner->SetPosition(position);


    // TODO 2.3 (1 linhas): Utilize a função mAcceleration.Set() para reinicializar a aceleração para zero.
    mAcceleration.Set(0.0f, 0.0f);

    float rot = mOwner->GetRotation();

    // TODO 2.4 (1 linha): Some à rotação atual do objeto (rot) a velocidade angular (mAngularSpeed)
    //  multiplicada pelo deltaTime.
    rot += mAngularSpeed * deltaTime;

    mOwner->SetRotation(rot);
}
