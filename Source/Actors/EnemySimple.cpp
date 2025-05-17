//
// Created by roger on 02/05/2025.
//

#include "EnemySimple.h"
#include "Actor.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

EnemySimple::EnemySimple(Game *game, float width, float height, float movespeed, float healthpoints)
    :Enemy(game, width, height, movespeed, healthpoints, 5.0f)
{
    mKnockBackSpeed = 800.0f * mGame->GetScale();
    mKnockBackDuration = 0.15f;
    mKnockBackTimer = mKnockBackDuration;
    mPlayerSpotted = false;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mWalkingAroundDuration = 2.0f;
    mWalkingAroundTimer = mWalkingAroundDuration;
    mWalkingAroundMooveSpeed = 50.0f * mGame->GetScale();

    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Goomba/Walk1.png", 64 * mGame->GetScale(), 64 * mGame->GetScale());
}

void EnemySimple::OnUpdate(float deltaTime) {

    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    ResolveGroundCollision();

    if (mPlayerSpotted) {
        MovementAfterPlayerSpotted();
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * mGame->GetScale() * deltaTime));

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000 * mGame->GetScale()) {
        SetPosition(Vector2::Zero);
    }
    // Se morreu
    if (Died()) {
        SetState(ActorState::Destroy);
    }
}

void EnemySimple::ResolveGroundCollision() {
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

void EnemySimple::MovementAfterPlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    if (GetPosition().x < player->GetPosition().x) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }
}

void EnemySimple::MovementBeforePlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    if (mWalkingAroundTimer > mWalkingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mWalkingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMooveSpeed, mRigidBodyComponent->GetVelocity().y));
    }

    // Testa se spottou player
    if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40 * mGame->GetScale()) { // Se está no mesmo nível verticalmente
        if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
        else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
    }
}
