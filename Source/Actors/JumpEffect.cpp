//
// Created by roger on 02/09/2025.
//

#include "JumpEffect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

JumpEffect::JumpEffect(Game *game, Actor *owner, float duration)
    :Actor(game)
    ,mWidth(130 * mGame->GetScale())
    ,mHeight(65 * mGame->GetScale())
    ,mEffectDuration(duration)
    ,mEffectTimer(mEffectDuration)
    ,mOwner(owner)
    ,mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Jump2/Jump.png",
                                           "../Assets/Sprites/Jump2/Jump.json",
                                           mWidth, mHeight, 1002);

    std::vector TakeOff = {12, 13, 14, 15, 16, 17, 18, 18};
    mDrawComponent->AddAnimation("TakeOff", TakeOff);

    std::vector Land = {4, 5, 6, 7, 8, 9, 10, 11, 11};
    mDrawComponent->AddAnimation("Land", Land);

    std::vector DoubleJump = {0, 1, 2, 3, 3};
    mDrawComponent->AddAnimation("DoubleJump", DoubleJump);

    mDrawComponent->SetAnimation("TakeOff");
    const float fps = 7.0f / mEffectDuration;
    mDrawComponent->SetAnimFPS(fps);

    // mDrawAnimatedComponent->SetTransparency(100);
    mDrawComponent->SetAlpha(0.4f);
}

void JumpEffect::OnUpdate(float deltaTime) {
    mEffectTimer += deltaTime;
    if (mEffectTimer >= mEffectDuration) {
        mDrawComponent->SetVisible(false);
        mState = ActorState::Paused;
    }
    else {
        mDrawComponent->SetVisible(true);
    }
}

void JumpEffect::StartEffect(EffectType type) {
    mEffectTimer = 0;
    // mDrawAnimatedComponent->ResetAnimationTimer();
    mDrawComponent->SetWidth(mWidth);
    mDrawComponent->SetHeight(mHeight);
    float fps;
    switch (type) {
        case EffectType::TakeOff:
            SetRotation(mOwner->GetRotation());
            SetScale(Vector2(mOwner->GetForward().x, 1));
            mDrawComponent->SetAnimation("TakeOff");
            fps = 7.0f / mEffectDuration;
            mDrawComponent->SetAnimFPS(fps);
            SetPosition(mOwner->GetPosition() + Vector2(mOwner->GetWidth() * 0.7f * -GetForward().x, mOwner->GetHeight() * 0.2f));
            break;

        case EffectType::Land:
            mDrawComponent->SetAnimation("Land");
            fps = 8.0f / mEffectDuration;
            mDrawComponent->SetAnimFPS(fps);
            SetPosition(mOwner->GetPosition() + Vector2(0, mOwner->GetHeight() * 0.2f));
            break;

        case EffectType::DoubleJump:
            mDrawComponent->SetAnimation("DoubleJump");
            fps = 4.0f / mEffectDuration;
            mDrawComponent->SetAnimFPS(fps);
            mDrawComponent->SetWidth(mWidth * 1.6f);
            mDrawComponent->SetHeight(mHeight * 1.6f);
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

    // mDrawAnimatedComponent->SetWidth(mWidth);
    // mDrawAnimatedComponent->SetHeight(mHeight);
}
