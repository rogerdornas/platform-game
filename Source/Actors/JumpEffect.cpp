//
// Created by roger on 02/09/2025.
//

#include "JumpEffect.h"
#include "../Game.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

JumpEffect::JumpEffect(Game *game, Actor *owner, float duration)
    :Actor(game)
    ,mWidth(130 * mGame->GetScale())
    ,mHeight(65 * mGame->GetScale())
    ,mEffectDuration(duration)
    ,mEffectTimer(mEffectDuration)
    ,mOwner(owner)
    ,mDrawAnimatedComponent(nullptr)
{
    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight,
                                           "../Assets/Sprites/Jump2/Jump.png",
                                           "../Assets/Sprites/Jump2/Jump.json", 1002);

    std::vector TakeOff = {12, 13, 14, 15, 16, 17, 18, 18};
    mDrawAnimatedComponent->AddAnimation("TakeOff", TakeOff);

    std::vector Land = {4, 5, 6, 7, 8, 9, 10, 11, 11};
    mDrawAnimatedComponent->AddAnimation("Land", Land);

    std::vector DoubleJump = {0, 1, 2, 3, 3};
    mDrawAnimatedComponent->AddAnimation("DoubleJump", DoubleJump);

    mDrawAnimatedComponent->SetAnimation("TakeOff");
    const float fps = 7.0f / mEffectDuration;
    mDrawAnimatedComponent->SetAnimFPS(fps);

    mDrawAnimatedComponent->SetTransparency(100);
}

void JumpEffect::OnUpdate(float deltaTime) {
    mEffectTimer += deltaTime;
    if (mEffectTimer >= mEffectDuration) {
        mDrawAnimatedComponent->SetIsVisible(false);
        mState = ActorState::Paused;
    }
    else {
        mDrawAnimatedComponent->SetIsVisible(true);
    }
}

void JumpEffect::StartEffect(EffectType type) {
    mEffectTimer = 0;
    mDrawAnimatedComponent->ResetAnimationTimer();
    mDrawAnimatedComponent->SetWidth(mWidth);
    mDrawAnimatedComponent->SetHeight(mHeight);
    float fps;
    switch (type) {
        case EffectType::TakeOff:
            SetRotation(mOwner->GetRotation());
            mDrawAnimatedComponent->SetAnimation("TakeOff");
            fps = 7.0f / mEffectDuration;
            mDrawAnimatedComponent->SetAnimFPS(fps);
            SetPosition(mOwner->GetPosition() + Vector2(mOwner->GetWidth() * 0.7f * -GetForward().x, mOwner->GetHeight() * 0.2f));
            break;

        case EffectType::Land:
            mDrawAnimatedComponent->SetAnimation("Land");
            fps = 8.0f / mEffectDuration;
            mDrawAnimatedComponent->SetAnimFPS(fps);
            SetPosition(mOwner->GetPosition() + Vector2(0, mOwner->GetHeight() * 0.2f));
            break;

        case EffectType::DoubleJump:
            mDrawAnimatedComponent->SetAnimation("DoubleJump");
            fps = 4.0f / mEffectDuration;
            mDrawAnimatedComponent->SetAnimFPS(fps);
            mDrawAnimatedComponent->SetWidth(mWidth * 1.6f);
            mDrawAnimatedComponent->SetHeight(mHeight * 1.6f);
            SetPosition(mOwner->GetPosition() + Vector2(0, mOwner->GetHeight() * 0.2f));
            break;
    }
}


void JumpEffect::ManageAnimations() {

}


void JumpEffect::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth);
    mDrawAnimatedComponent->SetHeight(mHeight);
}
