//
// Created by roger on 16/04/2025.
//

#pragma once
#include "Actor.h"
#include "../Actors/Particle.h"

class ParticleSystem : public Actor
{
public:
    ParticleSystem(class Game* game, float particleSize, float emitRate = 20.0f, float particleLifeTime = 0.7f, float lifeTime = 1.0f);

    void OnUpdate(float deltaTime) override;
    void SetParticleLifeTime(float time) { mParticleLifeTime = time; }
    void SetEmitRate(float rate) { mEmitRate = rate; }

    void SetEmitDirection(Vector2 emitDirection) { mEmitDirection = emitDirection; }
    void SetIsSplash(bool isSplash) { mIsSplash = isSplash; }
    void SetParticleSpeedScale(float speedScale);
    void SetParticleColor(SDL_Color color) { mColor = color; }
    void SetParticleGravity(bool g) { mParticleGravity = g; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void EmitParticle();

    float mEmitRate;  // partículas por segundo
    float mEmitTimer; // timer para controlar emissões
    float mParticleLifeTime;
    float mParticleSize;
    float mLifeTime;
    bool mIsSplash;
    SDL_Color mColor;
    float mParticleSpeedScale;
    bool mParticleGravity;
    Vector2 mEmitDirection;
};
