//
// Created by roger on 02/05/2025.
//

#include "EnemySimple.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"


EnemySimple::EnemySimple(Game* game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 5.0f)
{
    mMoneyDrop = 2;
    mKnockBackSpeed = 800.0f * mGame->GetScale();
    mKnockBackDuration = 0.15f;
    mKnockBackTimer = mKnockBackDuration;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mPatrolRadius = 50 * mGame->GetScale();
    mWalkingAroundDuration = 2.0f;
    mWalkingAroundTimer = mWalkingAroundDuration;
    mWalkingAroundMoveSpeed = 50.0f * mGame->GetScale();
    mGravity = 3000 * mGame->GetScale();

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.5f, mHeight * 1.5f, "../Assets/Sprites/Slime/Slime.png", "../Assets/Sprites/Slime/Slime.json", 999);
    std::vector walk = {0, 1, 2, 3, 4, 6};
    mDrawAnimatedComponent->AddAnimation("walk", walk);

    std::vector hit = {5};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("walk");
    mDrawAnimatedComponent->SetAnimFPS(8.0f);
}

void EnemySimple::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    if (mFlashTimer < mFlashDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    if (mPlayerSpotted) {
        mDrawAnimatedComponent->SetAnimFPS(15.0f);
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

        mGame->GetCamera()->StartCameraShake(0.3, mCameraShakeStrength);

        auto* blood = new ParticleSystem(mGame, 15, 300.0, 3.0, 0.07f);
        blood->SetPosition(GetPosition());
        blood->SetEmitDirection(Vector2::UnitY);
        blood->SetParticleSpeedScale(1.4);
        blood->SetParticleColor(SDL_Color{226, 90, 70, 255});
        blood->SetParticleGravity(true);

        auto* circleBlur = new Effect(mGame);
        circleBlur->SetDuration(1.0);
        circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 5.5f);
        circleBlur->SetEnemy(*this);
        circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
        circleBlur->SetEffect(TargetEffect::Circle);
        circleBlur->EnemyDestroyed();
    }
    ManageAnimations();
}

void EnemySimple::MovementAfterPlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    float playerX = player->GetPosition().x;
    float enemyX = GetPosition().x;

    // Verifica se o inimigo passou dos limites e deve inverter a direção
    if (enemyX < playerX - mPatrolRadius) {
        SetRotation(0.0); // anda para direita
    }
    else if (enemyX > playerX + mPatrolRadius) {
        SetRotation(Math::Pi); // anda para esquerda
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

void EnemySimple::ManageAnimations() {
    if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
    }
    else {
        mDrawAnimatedComponent->SetAnimation("walk");
    }
}


void EnemySimple::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mPatrolRadius = mPatrolRadius / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth * 1.5f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.5f);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.5f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.5f);
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

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
