//
// Created by roger on 16/04/2025.
//

#include "Particle.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Random.h"

Particle::Particle(Game* game, float lifeTime)
    :Actor(game)
    ,mDeathTimer(lifeTime)
    // ,mDrawComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
{
    // Componente visual
    Vector2 v1(0, 0);
    Vector2 v2(0, 1);
    Vector2 v3(1, 0);
    Vector2 v4(1, 1);
    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 0.1);

    // Define uma força aleatória para dar movimento à partícula
    Vector2 minForce = Vector2(-300.0f, -300.0f);
    Vector2 maxForce = Vector2(300.0f, 300.0f);
    Vector2 randForce = Random::GetVector(minForce, maxForce);
    mRigidBodyComponent->ApplyForce(randForce);
}

void Particle::OnUpdate(float deltaTime)
{
    mDeathTimer -= deltaTime;
    if (mDeathTimer <= 0.0f)
    {
        SetState(ActorState::Destroy);
    }
}