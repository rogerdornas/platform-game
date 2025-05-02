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
    mKnockBackTimer = 0.1f;
    mKnockBackDuration = 0.1f;
}

void FlyingEnemySimple::OnUpdate(float deltaTime) {

    mKnockBackTimer += deltaTime;

    Player* player = GetGame()->GetPlayer();

    float dx = player->GetPosition().x - GetPosition().x;
    float dy = player->GetPosition().y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += 2 * Math::Pi;
    }
    SetRotation(angle);

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

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mMoveSpeed));
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
