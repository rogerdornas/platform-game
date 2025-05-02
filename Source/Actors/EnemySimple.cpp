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
}

void EnemySimple::OnUpdate(float deltaTime) {
    Player* player = GetGame()->GetPlayer();

    if (GetPosition().x < player->GetPosition().x) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }


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

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 3000) {
        SetPosition(Vector2::Zero);
    }

    // Se morreu
    if (Died()) {
        SetState(ActorState::Destroy);
    }
}
