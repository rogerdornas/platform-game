//
// Created by roger on 09/06/2025.
//

#include "Projectile.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Projectile::Projectile(class Game *game, ProjectileType type, float width, float height, float speed, float damage)
    :Actor(game)
    ,mProjectileType(type)
    ,mWidth(width)
    ,mHeight(height)
    ,mSpeed(speed)
    ,mDamage(damage)
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    std::vector<int> firing;
    switch (mProjectileType) {
        case ProjectileType::Acid:
            mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.5f, mHeight * 1.5f, "../Assets/Sprites/AcidBlob/AcidBlob.png", "../Assets/Sprites/AcidBlob/AcidBlob.json", 1001);

            firing = {0, 1, 2, 3, 4, 5, 6};
            mDrawAnimatedComponent->AddAnimation("firing", firing);

            mDrawAnimatedComponent->SetAnimation("firing");
            mDrawAnimatedComponent->SetAnimFPS(8.0f);
            break;

        case ProjectileType::OrangeBall:
            mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.2f, mHeight * 2.2f, "../Assets/Sprites/FinalBossProjectile2/Projectile.png", "../Assets/Sprites/FinalBossProjectile2/Projectile.json", 1001);

            firing = {0, 1, 2, 3, 4, 5};
            mDrawAnimatedComponent->AddAnimation("firing", firing);

            mDrawAnimatedComponent->SetAnimation("firing");
            mDrawAnimatedComponent->SetAnimFPS(8.0f);
            break;
    }

    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {37, 218, 255, 255});
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Koopa/Shell.png", mWidth * 1.2, mHeight * 1.2);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddProjectile(this);
}

Projectile::~Projectile() {
    mGame->RemoveProjectile(this);
}

void Projectile::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        Deactivate();
    }
    else {
        Activate();
        ResolveGroundCollision();
        ResolvePlayerCollision();
    }
}

void Projectile::ExplosionEffect() {
    auto* explosion = new ParticleSystem(mGame, 12, 200.0, 0.2, 0.07f);
    explosion->SetPosition(GetPosition() + GetForward() * (mWidth / 2));
    explosion->SetEmitDirection(Vector2::Zero);
    explosion->SetIsSplash(true);
    explosion->SetParticleSpeedScale(1);
    if (mProjectileType == ProjectileType::Acid) {
        explosion->SetParticleColor(SDL_Color{208, 232, 92, 255});
    }
    if (mProjectileType == ProjectileType::OrangeBall) {
        explosion->SetParticleColor(SDL_Color{218, 147, 16, 255});
    }
    explosion->SetParticleGravity(false);
}

void Projectile::Activate() {
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

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
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth * 1.2f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.2f);
        mDrawSpriteComponent->SetIsVisible(true);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.5f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.5f);
        mDrawAnimatedComponent->SetIsVisible(true);
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
}

void Projectile::Deactivate() {
    SetState(ActorState::Paused);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mDurationTimer = 0;
    mAABBComponent->SetActive(false); // desativa colisão
    ExplosionEffect();

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(false);
    }
}

void Projectile::ResolveGroundCollision() {
    std::vector<Ground*> grounds;
    grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                Deactivate();
            }
        }
    }
}

void Projectile::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        player->ReceiveHit(mDamage, GetForward());
        Deactivate();
    }
}

void Projectile::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mSpeed = mSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth * 1.2f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.2f);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.5f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.5f);
    }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

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
