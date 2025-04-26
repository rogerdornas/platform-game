//
// Created by roger on 16/04/2025.
//

#include "ParticleSystem.h"
#include "../Actors/Particle.h"
#include "../Game.h"

ParticleSystem::ParticleSystem(Game* game, float emitRate, float particleLifeTime, float lifeTime)
    :Actor(game)
    ,mEmitRate(emitRate)
    ,mEmitTimer(0.0f)
    ,mParticleLifeTime(particleLifeTime)
    ,mLifeTime(lifeTime)
{
}

void ParticleSystem::OnUpdate(float deltaTime)
{

    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f)
    {
        SetState(ActorState::Destroy);
        return; // evita criar partículas depois de destruído
    }

    // Atualiza o timer de emissão
    mEmitTimer += deltaTime;

    // Emitir partículas de acordo com a taxa
    float interval = 1.0f / mEmitRate;
    while (mEmitTimer >= interval)
    {
        EmitParticle();
        mEmitTimer -= interval;
    }
}

void ParticleSystem::EmitParticle()
{
    Particle* p = new Particle(GetGame(), mParticleLifeTime);
    p->SetPosition(GetPosition());
}
