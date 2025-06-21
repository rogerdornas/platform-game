//
// Created by roger on 02/05/2025.
//

#include "FlyingEnemySimple.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

FlyingEnemySimple::FlyingEnemySimple(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 5.0f)
{
    mMoneyDrop = 4;
    mKnockBackSpeed = 1000.0f * mGame->GetScale();
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mFlyingAroundDuration = 1.0f;
    mFlyingAroundTimer = mFlyingAroundDuration;
    mFlyingAroundMoveSpeed = 100.0f * mGame->GetScale();

    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Koopa/WalkRight0.png",
    //                                                static_cast<int>(mWidth * 1.28),
    //                                                static_cast<int>(mHeight * 1.2));

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.0f, mHeight * 2.0f, "../Assets/Sprites/Beetle/Beetle.png", "../Assets/Sprites/Beetle/Beetle.json", 999);
    std::vector fly = {0, 1, 2, 3};
    mDrawAnimatedComponent->AddAnimation("fly", fly);

    std::vector hit = {4};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("fly");
    mDrawAnimatedComponent->SetAnimFPS(8.0f);
}

void FlyingEnemySimple::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

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
        MovementAfterPlayerSpotted(deltaTime);
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

void FlyingEnemySimple::MovementAfterPlayerSpotted(float deltaTime) {
    Player *player = GetGame()->GetPlayer();

    float dx = player->GetPosition().x - GetPosition().x;
    float dy = player->GetPosition().y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += 2 * Math::Pi;
    }

    SetRotation(angle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
    }
}

void FlyingEnemySimple::MovementBeforePlayerSpotted() {
    Player *player = GetGame()->GetPlayer();
    if (mFlyingAroundTimer > mFlyingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mFlyingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mFlyingAroundMoveSpeed));
    }

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }
}

void FlyingEnemySimple::ManageAnimations() {
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        mDrawAnimatedComponent->UseFlip(true);
        mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
    }
    else {
        mDrawAnimatedComponent->UseFlip(false);
    }

    if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
    }
    else {
        mDrawAnimatedComponent->SetAnimation("fly");
    }
}

void FlyingEnemySimple::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mFlyingAroundMoveSpeed = mFlyingAroundMoveSpeed / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth * 1.28f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.28f);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 2.0f);
        mDrawAnimatedComponent->SetHeight(mHeight * 2.0f);
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
