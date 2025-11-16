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
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

FlyingEnemySimple::FlyingEnemySimple(Game *game)
    :Enemy(game)
    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mFlyingAroundDuration(1.0f)
    ,mFlyingAroundTimer(0.0f)
    ,mFlyingAroundMoveSpeed(100.0f * mGame->GetScale())
{
    mWidth = 70 * mGame->GetScale();
    mHeight = 70 * mGame->GetScale();
    mMoveSpeed = 250 * mGame->GetScale();
    mHealthPoints = 30;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 4;
    mKnockBackSpeed = 1000.0f * mGame->GetScale();
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Beetle/Beetle.png",
                                                    "../Assets/Sprites/Beetle/Beetle.json",
                                                    mWidth * 2.0f, mHeight * 2.0f, 998);
    std::vector fly = {0, 1, 2, 3};
    mDrawComponent->AddAnimation("fly", fly);

    std::vector hit = {4};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(8.0f);
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
        if (mDrawComponent) {
            mDrawComponent->SetAnimFPS(15.0f);
        }
        MovementAfterPlayerSpotted(deltaTime);
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Se morreu
    if (Died()) {
    }

    if (mDrawComponent) {
        ManageAnimations();
    }
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
        SetScale(Vector2(GetScale().x * -1, 1));
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
        SetScale(Vector2(-1, 1));
        // mDrawAnimatedComponent->UseFlip(true);
        // mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
    }
    else {
        // mDrawAnimatedComponent->UseFlip(false);
        SetScale(Vector2(1, 1));
    }

    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else {
        mDrawComponent->SetAnimation("fly");
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 2.0f);
    //     mDrawAnimatedComponent->SetHeight(mHeight * 2.0f);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mColliderComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
