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
#include "../Components/DrawComponents/DrawPolygonComponent.h"


EnemySimple::EnemySimple(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 5.0f)
{
    mKnockBackSpeed = 800.0f * mGame->GetScale();
    mKnockBackDuration = 0.15f;
    mKnockBackTimer = mKnockBackDuration;
    mPlayerSpotted = false;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mWalkingAroundDuration = 2.0f;
    mWalkingAroundTimer = mWalkingAroundDuration;
    mWalkingAroundMoveSpeed = 50.0f * mGame->GetScale();
    mGravity = 3000 * mGame->GetScale();

    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Goomba/Walk1.png",
                                                    static_cast<int>(mWidth * 1.2),
                                                    static_cast<int>(mHeight * 1.2));}

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
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

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
                    mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
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
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMoveSpeed, mRigidBodyComponent->GetVelocity().y));
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

void EnemySimple::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;

    mDrawSpriteComponent->SetWidth(mWidth * 1.2f);
    mDrawSpriteComponent->SetHeight(mHeight * 1.2f);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);
}
