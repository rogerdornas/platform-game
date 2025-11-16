//
// Created by roger on 20/06/2025.
//

#include "Mantis.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

Mantis::Mantis(Game *game)
    :Enemy(game)
    ,mMantisState(State::WalkForward)
    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mWalkingAroundDuration(3.0f)
    ,mWalkingAroundTimer(0.0f)
    ,mWalkingAroundMoveSpeed(70 * mGame->GetScale())
    ,mGravity(3000 * mGame->GetScale())
    ,mDistToAttack(450.0f * mGame->GetScale())
    ,mJumpForce(-700.0f * mGame->GetScale())
    ,mAttackDuration(2.0f)
    ,mAttackTimer(0.0f)
    ,mWalkBackDuration(0.5f)
    ,mWalkBackTimer(0.0f)
    ,mWaitToAttackDuration(0.3f)
    ,mWaitToAttackTimer(0.0f)
{
    mWidth = 120 * mGame->GetScale();
    mHeight = 120 * mGame->GetScale();
    mMoveSpeed = 250 * mGame->GetScale();
    mHealthPoints = 90;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 10;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Mantis/Mantis.png",
                                                    "../Assets/Sprites/Mantis/Mantis.json",
                                                    1.35f * mWidth, 1.35f * mHeight, 998);
    std::vector walk = {8, 9, 10, 11};
    mDrawComponent->AddAnimation("walk", walk);

    std::vector attack = {1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector hit = {0};
    mDrawComponent->AddAnimation("hit", hit);

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(7.0f);
}

void Mantis::OnUpdate(float deltaTime) {
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
        if (mDrawComponent) {
            mDrawComponent->SetAnimFPS(10.0f);
        }
        MovementAfterPlayerSpotted(deltaTime);
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    // Se morreu
    if (Died()) {
    }

    if (mDrawComponent) {
        ManageAnimations();
    }
}

void Mantis::MovementBeforePlayerSpotted() {
    Player* player = GetGame()->GetPlayer();
    if (mWalkingAroundTimer > mWalkingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        SetScale(Vector2(GetScale().x * -1, 1));
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

void Mantis::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mMantisState) {
        case State::WalkForward:
            WalkForward(deltaTime);
            break;

        case State::WalkBack:
            WalkBack(deltaTime);
            break;

        case State::Attack:
            Attack(deltaTime);
            break;
    }
}

void Mantis::WalkForward(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }

    if (Math::Abs(dist) < mDistToAttack) {
        mWaitToAttackTimer += deltaTime;
        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
        }
        if (mWaitToAttackTimer >= mWaitToAttackDuration) {
            mWaitToAttackTimer = 0;
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mJumpForce));
            mMantisState = State::Attack;
            // if (mDrawAnimatedComponent) {
            //     mDrawAnimatedComponent->ResetAnimationTimer();
            // }
            mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
        }
    }
    else {
        mWaitToAttackTimer = 0;
    }
}

void Mantis::WalkBack(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mMoveSpeed * 2, mRigidBodyComponent->GetVelocity().y));
    }

    mWalkBackTimer += deltaTime;
    if (mWalkBackTimer >= mWalkBackDuration) {
        mWalkBackTimer = 0;
        mMantisState = State::WalkForward;
    }
}

void Mantis::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mMantisState = State::WalkBack;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mRigidBodyComponent->GetVelocity().y));
    }
    if (mRigidBodyComponent->GetVelocity().y == 0) {
        mAttackTimer = 0;
        mMantisState = State::WalkBack;
    }
}

void Mantis::ManageAnimations() {
    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
    else if (mMantisState == State::Attack) {
        mDrawComponent->SetAnimFPS(10.0f);
        mDrawComponent->SetAnimation("attack");
    }
    else {
        mDrawComponent->SetAnimation("walk");
    }
}

void Mantis::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;
    mDistToAttack = mDistToAttack / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 1.35f);
    //     mDrawAnimatedComponent->SetHeight(mHeight * 1.35f);
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





