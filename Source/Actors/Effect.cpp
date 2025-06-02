//
// Created by roger on 30/05/2025.
//

#include "Effect.h"
#include "../Components/DrawComponents/DrawParticleComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"


Effect::Effect(class Game *game)
    :Actor(game)
    ,mDuration(0.1f)
    ,mDurationTimer(0.0f)
    ,mSize(50 * mGame->GetScale())
    ,mEnemy(nullptr)
    ,mColor(SDL_Color{200, 200, 200, 255})
    ,mDrawSpriteComponent(nullptr)
    ,mDrawParticleComponent(nullptr)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
}

void Effect::SetEffect(TargetEffect targetEffect) {
    mTargetEffect = targetEffect;
    float size;
    switch (mTargetEffect) {
        case TargetEffect::swordHit:
            SetRotation(Random::GetFloatRange(-Math::Pi/4, Math::Pi/4));
            size = Random::GetFloatRange(1.2 * mSize, 1.5 * mSize);
            mDrawParticleComponent = new DrawParticleComponent(this, "../Assets/Sprites/Effects/Spark.png",
                                                                size * 2, size / 2, mColor);
        break;
        case TargetEffect::circle:
            SetPosition(mEnemy->GetPosition());
            mDrawParticleComponent = new DrawParticleComponent(this, "../Assets/Sprites/Effects/ImperfectCircleBlur.png",
                                                        mSize, mSize, mColor);
        break;
    }
}


void Effect::OnUpdate(float deltaTime) {
    switch (mTargetEffect) {
        case TargetEffect::bloodParticle:
            BloodParticleEffect(deltaTime);
            break;
        case TargetEffect::swordHit:
            SwordHitEffect(deltaTime);
            break;
        case TargetEffect::circle:
            Circle(deltaTime);
            break;
    }
}

void Effect::Circle(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    SDL_Color color = mColor;
    // if (mDurationTimer < mDuration / 2) {
    //     color.a = mDurationTimer / mDuration * mColor.a;
    // }
    // else {
    //     color.a = (1 - mDurationTimer / mDuration) * mColor.a;
    // }

    color.a = (1 - mDurationTimer / mDuration) * mColor.a;

    mDrawParticleComponent->SetColor(color);
    if (mEnemy) {
        SetPosition(mEnemy->GetPosition());
    }
}

void Effect::BloodParticleEffect(float deltaTime) {

}

void Effect::SwordHitEffect(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    mDrawParticleComponent->SetColor(mColor);

}

void Effect::ChangeResolution(float oldScale, float newScale) {
    mSize = mSize / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    switch (mTargetEffect) {
        case TargetEffect::swordHit:
            mDrawParticleComponent->SetWidth(mSize * 2);
            mDrawParticleComponent->SetHeight(mSize / 2);
        break;
        case TargetEffect::circle:
            mDrawParticleComponent->SetWidth(mSize);
            mDrawParticleComponent->SetHeight(mSize);
        break;
    }
}
