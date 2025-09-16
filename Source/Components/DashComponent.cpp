//
// Created by roger on 25/04/2025.
//

#include "DashComponent.h"
#include "../Actors/Actor.h"
#include "RigidBodyComponent.h"
#include "../Game.h"
#include "DrawComponents/DrawAnimatedComponent.h"

DashEffectActor::DashEffectActor(Game *game, Actor* owner, float duration)
    :Actor(game)
    ,mWidth(195 * mGame->GetScale())
    ,mHeight(159 * mGame->GetScale())
    ,mEffectDuration(duration)
    ,mEffectTimer(mEffectDuration)
    ,mOwner(owner)
    ,mDrawAnimatedComponent(nullptr)
{
    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight,
                                               "../Assets/Sprites/Dash5/Dash.png",
                                               "../Assets/Sprites/Dash5/Dash.json", 1002);

    std::vector idle = {0, 1, 2, 3, 4, 5};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(30);

    mDrawAnimatedComponent->SetTransparency(150);
}

void DashEffectActor::OnUpdate(float deltaTime) {
    mEffectTimer += deltaTime;
    if (mEffectTimer >= mEffectDuration) {
        mDrawAnimatedComponent->SetIsVisible(false);
    }
    else if (mOwner->GetState() == ActorState::Active) {
        mDrawAnimatedComponent->SetIsVisible(true);
        SetPosition(mOwner->GetPosition() - (Vector2(mOwner->GetWidth() * 1.5, 0) * GetForward().x));
    }
}

void DashEffectActor::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth);
    mDrawAnimatedComponent->SetHeight(mHeight);
}




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
    InitDashEffect();
}

void DashComponent::InitDashEffect() {
    mDashEffect = new DashEffectActor(mOwner->GetGame(), mOwner, mDashDuration);
}


bool DashComponent::UseDash(bool isOnGround) {
    if (mDashCooldownTimer <= 0 && !mIsDashing && (isOnGround || !mHasDashedInAir)) {
        mOwner->GetComponent<DrawAnimatedComponent>()->ResetAnimationTimer();
        mOwner->GetGame()->GetAudio()->PlayVariantSound("Dash/Dash.wav", 3);
        mIsDashing = true;
        mDashTimer = 0.0f;
        mDashCooldownTimer = mDashCooldown;
        // inicia animação do dash
        mDashEffect->SetRotation(mOwner->GetRotation());
        mDashEffect->SetPosition(mOwner->GetPosition() - (Vector2(mOwner->GetWidth() * 1.5, 0) * mDashEffect->GetForward().x));
        mDashEffect->StartDashEffect();

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
        return true;
    }
    return false;
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
    mDashEffect->StopDash();
}
