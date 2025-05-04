//
// Created by roger on 25/04/2025.
//

#include "DashComponent.h"
#include "../Actors/Actor.h"
#include "RigidBodyComponent.h"

DashComponent::DashComponent(class Actor *owner, float dashSpeed, float dashDuration, float dashCooldown)
    :Component(owner)
    ,mDashSpeed(dashSpeed)
    ,mDashDuration(dashDuration)
    ,mDashCooldown(dashCooldown)
{
    mIsDashing = false;
    mDashTimer = 0.0f;
    mDashCooldownTimer = 0.0f;
    mHasDashedInAir = false;
}


void DashComponent::UseDash(bool isOnGround) {
    if (mDashCooldownTimer <= 0 && !mIsDashing && (isOnGround || !mHasDashedInAir)) {
        mIsDashing = true;
        mDashTimer = 0.0f;
        mDashCooldownTimer = mDashCooldown;
        // mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mOwner->GetForward().x * mDashSpeed, 0));
        mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mOwner->GetForward().x * mDashSpeed, mOwner->GetComponent<RigidBodyComponent>()->GetVelocity().y));

        // Se estiver no ar, marca que já usou o dash
        if (!isOnGround) {
            mHasDashedInAir = true;
        }
    }
}


void DashComponent::Update(float deltaTime) {
    // Atualiza cooldown do dash
    if (mDashCooldownTimer > 0.0f) {
        mDashCooldownTimer -= deltaTime;
    }

    // Atualiza o dash em andamento
    if (mIsDashing) {
        mDashTimer += deltaTime;
        mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mOwner->GetForward().x * mDashSpeed, 0));

        if (mDashTimer >= mDashDuration) {
            mIsDashing = false;
            // Para não continuar deslizando
            mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, 0));
        }
    }
}
