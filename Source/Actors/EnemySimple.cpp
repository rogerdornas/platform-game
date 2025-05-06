//
// Created by roger on 02/05/2025.
//

#include "EnemySimple.h"
#include "Actor.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"

EnemySimple::EnemySimple(Game *game, float width, float height, float movespeed, float healthpoints)
    :Enemy(game, width, height, movespeed, healthpoints)
{
    mKnockBack = 300.0f;
    mKnockBackTimer = 0.15f;
    mKnockBackDuration = 0.1f;
    mPlayerSpotted = false;
    mDistToSpotPlayer = 300;
    mWalkingAroundTimer = 2.0f;
    mWalkingAroundDuration = 2.0f;
    mWalkingAroundMooveSpeed = 50.0f;
}

void EnemySimple::OnUpdate(float deltaTime) {

    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    // Colisao com ground e spines
    std::array<bool, 4> collisionSide;
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpine()) { // Colosão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveColision(*g->GetComponent<AABBComponent>());
                }
                else {
                    collisionSide[0] = false;
                    collisionSide[1] = false;
                    collisionSide[2] = false;
                    collisionSide[3] = false;
                }
                // colidiu top
                if (collisionSide[0]) {
                    if (mRigidBodyComponent->GetVelocity().y >= 0) {
                    }
                }
            }
            else if (g->GetIsSpine()) { // Colisão com spines
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    SetPosition(Vector2::Zero);
                }
            }
        }
    }

    Player* player = GetGame()->GetPlayer();

    if (mPlayerSpotted) {
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
    else {
        if (mWalkingAroundTimer > mWalkingAroundDuration) {
            SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
            mWalkingAroundTimer = 0;
        }
        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMooveSpeed, mRigidBodyComponent->GetVelocity().y));
        }

        // Testa se spottou player
        if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40) { // Se está no mesmo nível verticalmente
            if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
                mPlayerSpotted = true;
            }
            else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
                mPlayerSpotted = true;
            }
        }
    }

    // Gravidade
    // mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 3000) {
        SetPosition(Vector2::Zero);
    }

    // Se morreu
    if (Died()) {
        SetState(ActorState::Destroy);
    }
}
