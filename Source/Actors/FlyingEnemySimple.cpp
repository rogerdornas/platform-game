//
// Created by roger on 02/05/2025.
//

#include "FlyingEnemySimple.h"
#include "Actor.h"

#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"

FlyingEnemySimple::FlyingEnemySimple(Game *game, float width, float height, float movespeed, float healthpoints)
    :Enemy(game, width, height, movespeed, healthpoints)
{
    mKnockBack = 500.0f;
    mKnockBackTimer = 0.2f;
    mKnockBackDuration = 0.2f;
    mPlayerSpotted = false;
    mDistToSpotPlayer = 300;
    mFlyingAroundTimer = 1.0f;
    mFlyingAroundDuration = 1.0f;
    mFlyingAroundMooveSpeed = 100.0f;
}

void FlyingEnemySimple::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

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
    else {
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

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 3000) {
        SetPosition(Vector2::Zero);
    }

    // Se morreu
    if (Died()) {
        SetState(ActorState::Destroy);
    }
}
