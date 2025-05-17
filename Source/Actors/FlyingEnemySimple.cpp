//
// Created by roger on 02/05/2025.
//

#include "FlyingEnemySimple.h"
#include "Actor.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

FlyingEnemySimple::FlyingEnemySimple(Game *game, float width, float height, float movespeed, float healthpoints)
    :Enemy(game, width, height, movespeed, healthpoints, 5.0f)
{
    mKnockBackSpeed = 1000.0f * mGame->GetScale();
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;
    mPlayerSpotted = false;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mFlyingAroundDuration = 1.0f;
    mFlyingAroundTimer = mFlyingAroundDuration;
    mFlyingAroundMooveSpeed = 100.0f * mGame->GetScale();

    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Koopa/WalkRight0.png", 64 * mGame->GetScale(), 96 * mGame->GetScale());
}

void FlyingEnemySimple::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

    ResolveGroundCollision();

    if (mPlayerSpotted) {
        MovementAfterPlayerSpotted();
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000 * mGame->GetScale()) {
        SetPosition(Vector2::Zero);
    }

    // Se morreu
    if (Died()) {
        SetState(ActorState::Destroy);
    }
}

void FlyingEnemySimple::ResolveGroundCollision() {
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colosão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    mAABBComponent->ResolveColision(*g->GetComponent<AABBComponent>());
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    SetPosition(Vector2::Zero);
                }
            }
        }
    }
}

void FlyingEnemySimple::MovementAfterPlayerSpotted() {
    Player* player = GetGame()->GetPlayer();

    float dx = player->GetPosition().x - GetPosition().x;
    float dy = player->GetPosition().y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += 2 * Math::Pi;
    }
    SetRotation(angle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mMoveSpeed));
    }
}

void FlyingEnemySimple::MovementBeforePlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    if (mFlyingAroundTimer > mFlyingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mFlyingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mFlyingAroundMooveSpeed));
    }

    // Testa se spottou player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }
}
