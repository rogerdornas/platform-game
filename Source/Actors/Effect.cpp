//
// Created by roger on 30/05/2025.
//

#include "Effect.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"


Effect::Effect(class Game* game)
    :Actor(game)
    ,mDuration(0.1f)
    ,mDurationTimer(0.0f)
    ,mSize(50 * mGame->GetScale())
    ,mEnemy(nullptr)
    ,mColor(SDL_Color{200, 200, 200, 255})
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
{
}

void Effect::SetEffect(TargetEffect targetEffect) {
    mTargetEffect = targetEffect;
    float size;
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            SetTransformRotation(Random::GetFloatRange(-Math::Pi/4, Math::Pi/4));
            size = Random::GetFloatRange(1.2 * mSize, 1.5 * mSize);
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Effects/Spark.png", "",
                                                                size * 2, size / 2, 5000);
        break;
        case TargetEffect::Circle:
            SetPosition(mEnemy->GetPosition());
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Effects/ImperfectCircleBlur.png", "",
                                                        mSize, mSize, 5000);
        break;
    }

    if (mDrawComponent) {
        mDrawComponent->SetColor(Vector3(mColor.r / 255.0f, mColor.g / 255.0f, mColor.b / 255.0f));
        mDrawComponent->SetTextureFactor(0.0f);
    }
}


void Effect::OnUpdate(float deltaTime) {
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            SwordHitEffect(deltaTime);
            break;
        case TargetEffect::Circle:
            CircleEffect(deltaTime);
            break;
    }
}

void Effect::CircleEffect(float deltaTime) {
    if (mDrawComponent) {
        float alpha = (1 - mDurationTimer / mDuration) * mColor.a;
        mDrawComponent->SetAlpha(alpha / 255.0f);
    }

    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    if (mEnemy) {
        SetPosition(mEnemy->GetPosition());
    }
}

void Effect::SwordHitEffect(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }
}

void Effect::ChangeResolution(float oldScale, float newScale) {
    mSize = mSize / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            // mDrawParticleComponent->SetWidth(mSize * 2);
            // mDrawParticleComponent->SetHeight(mSize / 2);
        break;
        case TargetEffect::Circle:
            // mDrawParticleComponent->SetWidth(mSize);
            // mDrawParticleComponent->SetHeight(mSize);
        break;
    }
}
