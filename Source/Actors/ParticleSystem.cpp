//
// Created by roger on 16/04/2025.
//

#include "ParticleSystem.h"
#include "../Actors/Particle.h"
#include "../Game.h"

ParticleSystem::ParticleSystem(Game* game, float particleSize, float emitRate, float particleLifeTime, float lifeTime)
    : Actor(game)
    ,mEmitRate(emitRate)
    ,mEmitTimer(0.0f)
    ,mParticleLifeTime(particleLifeTime)
    ,mParticleSize(particleSize * mGame->GetScale())
    ,mLifeTime(lifeTime)
    ,mIsSplash(false)
    ,mColor(SDL_Color{255, 255, 255, 255})
    ,mParticleSpeedScale(1.0f * mGame->GetScale())
    ,mParticleGravity(true)
    ,mEmitDirection(Vector2::Zero)
{
}

void ParticleSystem::OnUpdate(float deltaTime) {
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0.0f) {
        SetState(ActorState::Destroy);
        return; // evita criar partículas depois de destruído
    }

    // Atualiza o timer de emissão
    mEmitTimer += deltaTime;

    // Emitir partículas de acordo com a taxa
    float interval = 1.0f / mEmitRate;
    while (mEmitTimer >= interval) {
        EmitParticle();
        mEmitTimer -= interval;
    }
}

void ParticleSystem::SetParticleSpeedScale(float speedScale) {
    mParticleSpeedScale = speedScale * mGame->GetScale();
}


void ParticleSystem::EmitParticle() {
    std::vector<Particle* > particles = mGame->GetParticles();
    for (Particle* p: particles) {
        if (p->GetState() == ActorState::Paused) {
            p->SetSize(mParticleSize);
            p->SetLifeDuration(mParticleLifeTime);
            p->SetIsSplash(mIsSplash);
            p->SetParticleColor(mColor);
            p->SetGravity(mParticleGravity);
            p->SetSpeedScale(mParticleSpeedScale);
            p->SetDirection(mEmitDirection);
            p->SetPosition(GetPosition());
            p->SetState(ActorState::Active);
            break;
        }
    }
}

void ParticleSystem::ChangeResolution(float oldScale, float newScale) {
    mParticleSize = mParticleSize / oldScale * newScale;
    mParticleSpeedScale = mParticleSpeedScale / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
}
