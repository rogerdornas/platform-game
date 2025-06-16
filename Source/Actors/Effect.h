//
// Created by roger on 30/05/2025.
//

#pragma once

#include "Actor.h"
#include "../Game.h"

enum class TargetEffect {
    SwordHit,
    Circle
};

class Effect : public Actor
{
public:
    Effect(class Game* game);

    void SetEffect(TargetEffect targetEffect);
    void SetSize(float size) { mSize = size; }
    void SetColor(SDL_Color color) { mColor = color; }
    void SetEnemy(Enemy &enemy) { mEnemy = &enemy; }
    void SetDuration(float duration) { mDuration = duration; }
    void EnemyDestroyed() { mEnemy = nullptr; };

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

private:
    void SwordHitEffect(float deltaTime);
    void CircleEffect(float deltaTime);

    TargetEffect mTargetEffect;

    float mDuration;
    float mDurationTimer;
    float mSize;
    Enemy* mEnemy;

    SDL_Color mColor;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawSpriteComponent* mDrawSpriteComponent;
    class DrawParticleComponent* mDrawParticleComponent;
    class DrawAnimatedComponent* mDrawAnimatedComponent;
};
