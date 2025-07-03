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
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Mantis::Mantis(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 15.0f)
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
    mMoneyDrop = 10;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, 1.25f * mWidth, 1.25f * mHeight, "../Assets/Sprites/Mantis/Mantis.png", "../Assets/Sprites/Mantis/Mantis.json", 999);
    std::vector walk = {8, 9, 10, 11};
    mDrawAnimatedComponent->AddAnimation("walk", walk);

    std::vector attack = {1, 2, 3, 4, 5, 6, 7};
    mDrawAnimatedComponent->AddAnimation("attack", attack);

    std::vector hit = {0};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("walk");
    mDrawAnimatedComponent->SetAnimFPS(7.0f);
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
        mDrawAnimatedComponent->SetAnimFPS(10.0f);
        MovementAfterPlayerSpotted(deltaTime);
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

void Mantis::MovementBeforePlayerSpotted() {
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
    }
    else {
        SetRotation(Math::Pi);
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }

    if (Math::Abs(dist) < mDistToAttack) {
        mWaitToAttackTimer += deltaTime;
        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
        if (mWaitToAttackTimer >= mWaitToAttackDuration) {
            mWaitToAttackTimer = 0;
            if (mKnockBackTimer >= mKnockBackDuration) {
                mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mJumpForce));
            }
            mMantisState = State::Attack;
            mDrawAnimatedComponent->ResetAnimationTimer();
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
    }
    else {
        SetRotation(Math::Pi);
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
        mDrawAnimatedComponent->SetAnimation("hit");
    }
    else if (mMantisState == State::Attack) {
        mDrawAnimatedComponent->SetAnimFPS(10.0f);
        mDrawAnimatedComponent->SetAnimation("attack");
    }
    else {
        mDrawAnimatedComponent->SetAnimation("walk");
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

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.25f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.25f);
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





