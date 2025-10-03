//
// Created by roger on 01/09/2025.
//

#include "Lava.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Lava::Lava(class Game *game, float width, float height, bool isMoving, float movingDuration, Vector2 velocity)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDamage(20.0f)
    ,mIsMoving(isMoving)
    ,mMovingDuration(movingDuration)
    ,mMovingTimer(0.0f)
    ,mVelocity(velocity * mGame->GetScale())
    ,mSwordHitLava(false)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {255, 0, 0, 255});

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight * 1.1f, "../Assets/Sprites/Lava/Lava.png", "../Assets/Sprites/Lava/Lava.json", 98);

    std::vector<int> idle = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(9.0f);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 40000);
    mAABBComponent = new AABBComponent(this, v1, v3);

    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }
}

void Lava::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
    }

    ResolveEnemyCollision();
    ResolvePlayerCollision();
    ResolveSwordCollision();
}

void Lava::SetIsMoving(bool isMoving) {
    mIsMoving = isMoving;
    if (mIsMoving == true) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void Lava::ResolvePlayerCollision() {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        player->SetIsInvulnerable(false);
        player->ReceiveHit(mDamage, Vector2::UnitY);
        if (!player->Died()) {
            player->GetComponent<AABBComponent>()->SetActive(false);
            player->SetInvulnerableTimer(-1.0f);
            if (player->GetComponent<DrawAnimatedComponent>()) {
                player->GetComponent<DrawAnimatedComponent>()->SetAnimation("hurt");
            }
            player->SetState(ActorState::Paused);
            mGame->InitCrossFade(1.5f);
            mGame->SetHitByLava();
            mGame->SetLavaRespawnPosition(mRespawnPosition);
        }

        auto* fireBubble = new ParticleSystem(mGame, 10, 180.0f, 0.5f, 0.07f);
        fireBubble->SetPosition(Vector2(player->GetPosition().x, GetPosition().y - mHeight / 2));
        fireBubble->SetEmitDirection(Vector2::NegUnitY);
        fireBubble->SetIsSplash(true);
        fireBubble->SetParticleSpeedScale(0.7);
        SDL_Color color = {255, 0, 0, 255};
        fireBubble->SetParticleColor(color);
        fireBubble->SetParticleGravity(true);
    }
}

void Lava::ResolveEnemyCollision() {
    std::vector<Enemy*> enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e : enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                e->ReceiveHit(mDamage * 1000, Vector2::UnitY);

                auto* fireBubble = new ParticleSystem(mGame, 10, 180.0f, 0.5f, 0.07f);
                fireBubble->SetPosition(Vector2(e->GetPosition().x, GetPosition().y - mHeight / 2));
                fireBubble->SetEmitDirection(Vector2::NegUnitY);
                fireBubble->SetIsSplash(true);
                fireBubble->SetParticleSpeedScale(0.7);
                SDL_Color color = {255, 0, 0, 255};
                fireBubble->SetParticleColor(color);
                fireBubble->SetParticleGravity(true);
            }
        }
    }
}

void Lava::ResolveSwordCollision() {
    Sword* sword = mGame->GetPlayer()->GetSword();
    if (sword->GetComponent<DrawAnimatedComponent>() && !sword->GetComponent<DrawAnimatedComponent>()->IsVisible()) {
        mSwordHitLava = false;
    }
    if (sword->GetComponent<ColliderComponent>()->Intersect(*mAABBComponent)) {
        if (!mSwordHitLava) {
            Vector2 collisionNormal = sword->GetComponent<ColliderComponent>()->CollisionSide(*mAABBComponent);
            if (1)
            {
                auto* fireBubble = new ParticleSystem(mGame, 10, 180.0f, 0.5f, 0.07f);
                if (collisionNormal == Vector2::NegUnitY) {
                    fireBubble->SetPosition(Vector2(sword->GetPosition().x, GetPosition().y - GetHeight() / 2));
                }
                if (collisionNormal == Vector2::UnitY) {
                    fireBubble->SetPosition(Vector2(sword->GetPosition().x, GetPosition().y + GetHeight() / 2));
                }
                if (collisionNormal == Vector2::NegUnitX) {
                    fireBubble->SetPosition(Vector2(GetPosition().x - GetWidth() / 2, GetPosition().y));
                }
                if (collisionNormal == Vector2::UnitX) {
                    fireBubble->SetPosition(Vector2(GetPosition().x + GetWidth() / 2, GetPosition().y));
                }

                fireBubble->SetEmitDirection(sword->GetForward() * -1);
                fireBubble->SetIsSplash(true);
                fireBubble->SetParticleSpeedScale(0.7f);
                SDL_Color color = {255, 0, 0, 255};
                fireBubble->SetParticleColor(color);
                fireBubble->SetParticleGravity(true);
                mSwordHitLava = true;
            }
        }
    }
}


void Lava::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mRespawnPosition.x = mRespawnPosition.x / oldScale * newScale;
    mRespawnPosition.y = mRespawnPosition.y / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mVelocity.x = mVelocity.x / oldScale * newScale;
    mVelocity.y = mVelocity.y / oldScale * newScale;

    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.1f);
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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
