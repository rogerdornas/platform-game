//
// Created by roger on 16/04/2025.
//

#include "Particle.h"
#include "../Actors/ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawParticleComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Particle::Particle(Game* game)
    :Actor(game)
    ,mSize(8.0f * mGame->GetScale())
    ,mLifeTDuration(0.0f)
    ,mLifeTimer(0.0f)
    ,mIsSplash(false)
    ,mTexturePath("../Assets/Sprites/Particle/Ellipse.png")
    ,mColor(SDL_Color{255, 255, 255, 255})
    ,mGravity(true)
    ,mGravityForce(2000.0f)
    ,mDirection(Vector2::Zero)
    ,mSpeedScale(1.0f * mGame->GetScale())
    ,mDrawPolygonComponent(nullptr)
    ,mDrawParticleComponent(nullptr)
    ,mRigidBodyComponent(nullptr)
    ,mAABBComponent(nullptr)
{
    float size = Random::GetFloatRange(mSize * 0.5, mSize * 1.5);
    float width = 1.2 * size;
    float height = size;

    // Componente visual
    Vector2 v1(-width/2, -height/2);
    Vector2 v2(width/2, -height/2);
    Vector2 v3(width/2, height/2);
    Vector2 v4(-width/2, height/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 255, 255}, 5000);

    mDrawParticleComponent = new DrawParticleComponent(this, mTexturePath,
                                                    static_cast<int>(width * 1.6),
                                                    static_cast<int>(height * 1.6),
                                                    mColor,
                                                    5000);
    mRigidBodyComponent = new RigidBodyComponent(this, 0.1);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddParticle(this);
}

Particle::~Particle() {
    mGame->RemoveParticle(this);
}

void Particle::SetSize(float size) {
    mSize = Random::GetFloatRange(size * 0.5, size * 1.5);
}

void Particle::SetDirection(Vector2 direction) {
    mDirection = direction;
    if (mDirection.x == 1.0f) {
        // Right
        Vector2 minForce = Vector2(300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(800.0f * mSpeedScale, -140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.x == -1.0f) {
        // Left
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(-800.0f * mSpeedScale, -140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.y == -1.0f) {
        // Up
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -1300.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, -450.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else if (mDirection.y == 1.0f) {
        // down
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -800.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, 140.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
    else {
        Vector2 minForce = Vector2(-300.0f * mSpeedScale, -300.0f * mSpeedScale);
        Vector2 maxForce = Vector2(300.0f * mSpeedScale, 300.0f * mSpeedScale);

        Vector2 randForce = Random::GetVector(minForce, maxForce);
        mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + randForce);
    }
}

void Particle::OnUpdate(float deltaTime)
{
    mLifeTimer += deltaTime;
    if (mLifeTimer >= mLifeTDuration) {
        Deactivate();
    }
    else {
        Activate();
        // Rotation
        Vector2 velocity = mRigidBodyComponent->GetVelocity();
        if (velocity.Length() != 0) {
            velocity.Normalize();
        }
        SetRotation(Math::Atan2(velocity.y, velocity.x));

        // Gravidade
        if (mGravity) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                     mRigidBodyComponent->GetVelocity().y +
                                                     mGravityForce * deltaTime));
        }

        if (!mIsSplash) {
            std::vector<Ground*> grounds = mGame->GetGrounds();
            if (!grounds.empty()) {
                for (Ground* g : grounds) {
                    if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                        Deactivate();
                        auto* blood = new ParticleSystem(mGame, 6, 100.0, 0.09, 0.05f);
                        blood->SetPosition(GetPosition());
                        blood->SetIsSplash(true);
                        blood->SetParticleSpeedScale(1);
                        blood->SetParticleColor(mColor);
                        blood->SetParticleGravity(true);
                    }
                }
            }
        }
    }
}

void Particle::Activate() {
    if (mDrawParticleComponent) {
        mDrawParticleComponent->SetColor(mColor);
    }

    float width = 1.2 * mSize;
    float height = mSize;

    // Componente visual
    Vector2 v1(-width/2, -height/2);
    Vector2 v2(width/2, -height/2);
    Vector2 v3(width/2, height/2);
    Vector2 v4(-width/2, height/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
        mDrawPolygonComponent->SetIsVisible(true);
    }
    if (mDrawParticleComponent) {
        mDrawParticleComponent->SetWidth(width * 1.6);
        mDrawParticleComponent->SetHeight(height * 1.6);
        mDrawParticleComponent->SetIsVisible(true);
    }
}


void Particle::Deactivate() {
    SetState(ActorState::Paused);
    mLifeTimer = 0;
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mAABBComponent->SetActive(false); // desativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawParticleComponent) {
        mDrawParticleComponent->SetIsVisible(false);
    }
}

void Particle::ChangeResolution(float oldScale, float newScale) {
    mSize = mSize / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mSpeedScale = mSpeedScale / oldScale * newScale;
    mGravityForce = mGravityForce / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    float width = 1.2 * mSize;
    float height = mSize;
    mDrawParticleComponent->SetWidth(width * 1.6f);
    mDrawParticleComponent->SetHeight(height * 1.6f);

    Vector2 v1(-width / 2, -height / 2);
    Vector2 v2(width / 2, -height / 2);
    Vector2 v3(width / 2, height / 2);
    Vector2 v4(-width / 2, height / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
