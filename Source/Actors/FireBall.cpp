//
// Created by roger on 29/04/2025.
//

#include "FireBall.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/AABBComponent.h"

FireBall::FireBall(class Game *game)
    :Actor(game)
    ,mWidth(50.0f)
    ,mHeight(50.0f)
    ,mSpeed(1600.0f)
    ,mDuration(3.0f)
    ,mDurationTimer(0.0f)
    ,mDamage(20)
{
    mDurationTimer = mDuration;

    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, false, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3, {37, 218, 255, 255});

    game->AddFireBall(this);
}

FireBall::~FireBall() {
    mGame->RemoveFireball(this);
}

void FireBall::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        SetState(ActorState::Paused);
        mAABBComponent->SetActive(false); // desativa colisão
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mDurationTimer = 0;
    }
    else {
        mAABBComponent->SetActive(true); // reativa colisão
        mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);

        // Colisão com enemys
        std::vector<Enemy*> enemys;
        enemys = GetGame()->GetEnemys();
        if (!enemys.empty()) {
            for (Enemy* e : enemys) {
                if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                    e->ReceiveHit(mDamage, GetForward());
                    SetState(ActorState::Paused);
                    mAABBComponent->SetActive(false); // desativa colisão
                    mRigidBodyComponent->SetVelocity(Vector2::Zero);
                    mDurationTimer = 0;
                }
            }
        }
    }
}
