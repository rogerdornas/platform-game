//
// Created by roger on 09/06/2025.
//

#include "Projectile.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Projectile::Projectile(class Game *game, ProjectileType type, float width, float height, float speed, float damage)
    :Actor(game)
    ,mProjectileType(type)
    ,mWidth(width)
    ,mHeight(height)
    ,mSpeed(speed)
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
    ,mDamage(damage)
    ,mMaxBounce(0)
    ,mBounceCount(0)
    ,mCollisionCooldownDuration(0.1f)
    ,mCollisionCooldownTimer(0.0f)
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
{
    std::vector<int> firing;
    switch (mProjectileType) {
        case ProjectileType::Acid:
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/AcidBlob/AcidBlob.png",
                                                        "../Assets/Sprites/AcidBlob/AcidBlob.json",
                                                        mWidth * 1.5f, mHeight * 1.5f, 1001);

            firing = {0, 1, 2, 3, 4, 5, 6};
            mDrawComponent->AddAnimation("firing", firing);

            mDrawComponent->SetAnimation("firing");
            mDrawComponent->SetAnimFPS(8.0f);
            break;

        case ProjectileType::OrangeBall:
            mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/FinalBossProjectile2/Projectile.png",
                                                        "../Assets/Sprites/FinalBossProjectile2/Projectile.json",
                                                        mWidth * 2.2f, mHeight * 2.2f, 1001);

            firing = {0, 1, 2, 3, 4, 5};
            mDrawComponent->AddAnimation("firing", firing);

            mDrawComponent->SetAnimation("firing");
            mDrawComponent->SetAnimFPS(8.0f);
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

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddProjectile(this);
}

Projectile::~Projectile() {
    mGame->RemoveProjectile(this);
}

void Projectile::OnUpdate(float deltaTime) {
    if (mCollisionCooldownTimer < mCollisionCooldownDuration) {
        mCollisionCooldownTimer += deltaTime;
    }

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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    mAABBComponent->SetActive(true); // reativa colisão
    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
        mRectComponent->SetVisible(true);
    }

    if (mDrawComponent) {
        mDrawComponent->SetWidth(mWidth * 1.5f);
        mDrawComponent->SetHeight(mHeight * 1.5f);
        mDrawComponent->SetVisible(true);
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
}

void Projectile::Deactivate() {
    SetState(ActorState::Paused);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mDuration = 3.0f;
    mDurationTimer = 0;
    mMaxBounce = 0;
    mBounceCount = 0;
    mAABBComponent->SetActive(false); // desativa colisão
    ExplosionEffect();

    if (mRectComponent) {
        mRectComponent->SetVisible(false);
    }

    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
    }
}

void Projectile::ResolveGroundCollision() {
    Vector2 collisionNormal;
    std::vector<Ground*> grounds;
    grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                if (mBounceCount < mMaxBounce) {
                    if (mCollisionCooldownTimer >= mCollisionCooldownDuration) {
                        collisionNormal = mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());

                        // Colidiu Top ou Bot
                        if (collisionNormal == Vector2::NegUnitY || collisionNormal == Vector2::UnitY) {
                            SetRotation(-GetRotation());
                            SetTransformRotation(-GetTransformRotation());
                        }

                        // Colidiu Left ou Right
                        if (collisionNormal == Vector2::NegUnitX || collisionNormal == Vector2::UnitX) {
                            SetRotation(Math::Pi - GetRotation());
                            SetTransformRotation(Math::Pi - GetTransformRotation());
                        }

                        mBounceCount++;
                        mCollisionCooldownTimer = 0;
                    }
                }
                else {
                    Deactivate();
                }
            }
        }
    }
}

void Projectile::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 1.5f);
    //     mDrawAnimatedComponent->SetHeight(mHeight * 1.5f);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
