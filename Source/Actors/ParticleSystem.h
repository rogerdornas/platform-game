//
// Created by roger on 16/04/2025.
//

#pragma once
#include "Actor.h"
#include "../Actors/Particle.h"

class ParticleSystem : public Actor
{
public:
    ParticleSystem(class Game* game, Particle::ParticleType particleType, float particleSize, float emitRate = 20.0f, float particleLifeTime = 0.7f, float lifeTime = 1.0f);

    void OnUpdate(float deltaTime) override;
    void SetParticleLifeTime(float time) { mParticleLifeTime = time; }
    void SetEmitRate(float rate) { mEmitRate = rate; }
    void EndParticleSystem() { mLifeTime = 0; }

    void SetEmitDirection(Vector2 emitDirection) { mEmitDirection = emitDirection; }
    void SetGroundCollision(bool groundCollision) { mGroundCollision = groundCollision; }
    void SetEnemyCollision(bool enemyCollision) { mEnemyCollision = enemyCollision; }
    void SetParticleSpeedScale(float speedScale);
    void SetParticleColor(SDL_Color color) { mColor = color; }
    void SetParticleGravity(bool g) { mParticleGravity = g; }

    void SetApplyDamage(bool applyDamage) { mApplyDamage = applyDamage; }
    void SetApplyFreeze(bool applyFreeze) { mApplyFreeze = applyFreeze; }
    void SetFreezeDamage(float freezeDamage) { mFreezeDamage = freezeDamage; }
    void SetFreezeIntensity(float freezeIntensity) { mFreezeIntensity = freezeIntensity; }

    void SetConeSpread(float degrees) { mConeSpread = degrees; }
    void SetParticleDrawOrder(int particleDrawOrder) { mParticleDrawOrder = particleDrawOrder; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void EmitParticle();

    Particle::ParticleType mParticleType;
    float mEmitRate;  // partículas por segundo
    float mEmitTimer; // timer para controlar emissões
    float mParticleLifeTime;
    float mParticleSize;
    float mLifeTime;
    bool mGroundCollision;
    bool mEnemyCollision;
    SDL_Color mColor;
    float mParticleSpeedScale;
    bool mParticleGravity;
    Vector2 mEmitDirection;

    bool mApplyDamage;
    bool mApplyFreeze;
    float mFreezeDamage;
    float mFreezeIntensity;

    float mConeSpread;
    int mParticleDrawOrder;
};
