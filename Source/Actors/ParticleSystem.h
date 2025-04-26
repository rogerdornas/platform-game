//
// Created by roger on 16/04/2025.
//

#pragma once
#include "Actor.h"
#include "../Actors/Particle.h"

class ParticleSystem : public Actor
{
public:
    ParticleSystem(class Game* game, float emitRate = 20.0f, float particleLifeTime = 0.7f, float lifeTime = 1.0f);

    void OnUpdate(float deltaTime) override;

    void SetParticleLifeTime(float time) { mParticleLifeTime = time; }
    void SetEmitRate(float rate) { mEmitRate = rate; }

private:
    void EmitParticle();

    std::vector<Particle*> mParticles;

    float mEmitRate;           // partículas por segundo
    float mEmitTimer;          // timer para controlar emissões
    float mParticleLifeTime;
    float mLifeTime;
};
