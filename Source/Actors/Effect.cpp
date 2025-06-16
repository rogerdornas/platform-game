//
// Created by roger on 30/05/2025.
//

#include "Effect.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawParticleComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"


Effect::Effect(class Game* game)
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
        case TargetEffect::SwordHit:
            SetRotation(Random::GetFloatRange(-Math::Pi/4, Math::Pi/4));
            size = Random::GetFloatRange(1.2 * mSize, 1.5 * mSize);
            mDrawParticleComponent = new DrawParticleComponent(this, "../Assets/Sprites/Effects/Spark.png",
                                                                size * 2, size / 2, mColor);
        break;
        case TargetEffect::Circle:
            SetPosition(mEnemy->GetPosition());
            mDrawParticleComponent = new DrawParticleComponent(this, "../Assets/Sprites/Effects/ImperfectCircleBlur.png",
                                                        mSize, mSize, mColor);
        break;
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
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Destroy);
        return;
    }

    SDL_Color color = mColor;
    color.a = (1 - mDurationTimer / mDuration) * mColor.a;

    mDrawParticleComponent->SetColor(color);
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

    mDrawParticleComponent->SetColor(mColor);
}

void Effect::ChangeResolution(float oldScale, float newScale) {
    mSize = mSize / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    switch (mTargetEffect) {
        case TargetEffect::SwordHit:
            mDrawParticleComponent->SetWidth(mSize * 2);
            mDrawParticleComponent->SetHeight(mSize / 2);
        break;
        case TargetEffect::Circle:
            mDrawParticleComponent->SetWidth(mSize);
            mDrawParticleComponent->SetHeight(mSize);
        break;
    }
}
