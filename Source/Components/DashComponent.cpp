//
// Created by roger on 25/04/2025.
//

#include "DashComponent.h"
#include "../Actors/Actor.h"
#include "RigidBodyComponent.h"
#include "../Game.h"

DashComponent::DashComponent(class Actor* owner, float dashSpeed, float dashDuration, float dashCooldown)
    :Component(owner)
    ,mIsDashing(false)
    ,mDashSpeed(dashSpeed)
    ,mDashDuration(dashDuration)
    ,mDashCooldown(dashCooldown)
    ,mDashTimer(0.0f)
    ,mDashCooldownTimer(0.0f)
    ,mHasDashedInAir(false)
{
}

void DashComponent::UseDash(bool isOnGround) {
    if (mDashCooldownTimer <= 0 && !mIsDashing && (isOnGround || !mHasDashedInAir)) {
        mOwner->GetGame()->GetAudio()->PlayVariantSound("Dash/Dash.wav", 3);
        mIsDashing = true;
        mDashTimer = 0.0f;
        mDashCooldownTimer = mDashCooldown;
        // Se estiver no ar, define a velocidade y para 0
        if (!isOnGround) {
            mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mOwner->GetForward().x * mDashSpeed, 0));
        }
        else { // Se estiver no chão, realiza o dash levando em conta a velocidade y, útil para plataformas móveis
            mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(
                mOwner->GetForward().x * mDashSpeed, mOwner->GetComponent<RigidBodyComponent>()->GetVelocity().y));
        }
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
        mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(mOwner->GetForward().x * mDashSpeed,
                                                                        mOwner->GetComponent<RigidBodyComponent>()->
                                                                        GetVelocity().y));

        if (mDashTimer >= mDashDuration) {
            mIsDashing = false;
            // Para não continuar deslizando
            mOwner->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(0, 0));
        }
    }
}

void DashComponent::StopDash() {
    mIsDashing = false;
    mDashTimer = mDashDuration;
}
