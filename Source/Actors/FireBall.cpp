//
// Created by roger on 29/04/2025.
//

#include "FireBall.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"


FireBall::FireBall(class Game *game)
    :Actor(game)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

    ,mWidth(50.0f)
    ,mHeight(50.0f)
    ,mSpeed(1800.0f)
    ,mDuration(3.0f)
    ,mDurationTimer(mDuration)
    ,mDamage(20)
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

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {37, 218, 255, 255});
    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Koopa/Shell.png", mWidth + 30, mHeight + 30);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);

    game->AddFireBall(this);
}

FireBall::~FireBall() {
    mGame->RemoveFireball(this);
}

void FireBall::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        Deactivate();
    }
    else {
        Activate();
        ResolveGroundCollision();
        ResolveEnemyCollision();
    }
}

void FireBall::Activate() {
    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(true);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(true);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(true);
    }
    mRigidBodyComponent->SetVelocity(GetForward() * mSpeed);
}

void FireBall::Deactivate() {
    SetState(ActorState::Paused);
    mAABBComponent->SetActive(false); // desativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }
    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(false);
    }
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mDurationTimer = 0;
}

void FireBall::ResolveGroundCollision() {
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                Deactivate();
            }
        }
    }
}

void FireBall::ResolveEnemyCollision() {
    std::vector<Enemy*> enemys;
    enemys = GetGame()->GetEnemys();
    if (!enemys.empty()) {
        for (Enemy* e : enemys) {
            if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                e->ReceiveHit(mDamage, GetForward());
                Deactivate();
            }
        }
    }
}
