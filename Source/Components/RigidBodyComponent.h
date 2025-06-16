//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once

#include "Component.h"
#include "../Math.h"

class RigidBodyComponent : public Component
{
public:
    // Lower update order to update first
    RigidBodyComponent(class Actor* owner, float mass = 1.0f, float maxSpeedX = 10000, float maxSpeedY = 10000, int updateOrder = 10);

    void Update(float deltaTime) override;

    const Vector2 &GetVelocity() const { return mVelocity; }
    void SetVelocity(const Vector2 &velocity) { mVelocity = velocity; }

    const Vector2 &GetAcceleration() const { return mAcceleration; }
    void SetAcceleration(const Vector2 &acceleration) { mAcceleration = acceleration; }

    void SetAngularSpeed(const float speed) { mAngularSpeed = speed; }
    float GetAngularSpeed() const { return mAngularSpeed; }

    void SetMaxSpeedX(float v) { mMaxSpeedX = v; }
    void SetMaxSpeedY(float v) { mMaxSpeedY = v; }
    float GetMaxSpeedX() const { return mMaxSpeedX; }
    float GetMaxSpeedY() const { return mMaxSpeedY; }

    void ApplyForce(const Vector2 &force);

private:
    // Physical properties
    float mMass;
    float mAngularSpeed;
    float mMaxSpeedX;
    float mMaxSpeedY;

    Vector2 mVelocity;
    Vector2 mAcceleration;
};
