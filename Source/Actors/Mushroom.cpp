//
// Created by roger on 07/10/2025.
//

#include "Mushroom.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Mushroom::Mushroom(Game *game)
    :Enemy(game)
    ,mMushroomState(State::Stop)

    ,mStopDuration(0.7f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)

    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mLookingAroundDuration(2.5f)
    ,mLookingAroundTimer(0.0f)

    ,mWalkAwayDuration(0.4f)
    ,mWalkAwayTimer(0.0f)

    ,mGravity(3000 * mGame->GetScale())

    ,mDistToAttack(300.0f * mGame->GetScale())
    ,mJumpForce(-600.0f * mGame->GetScale())
    ,mAttackDuration(0.7f)
    ,mAttackTimer(0.0f)

    ,mStumDuration(1.2f)
    ,mStumTimer(0.0f)
{
    mWidth = 80 * mGame->GetScale();
    mHeight = 110 * mGame->GetScale();
    mMoveSpeed = 300 * mGame->GetScale();
    mHealthPoints = 500;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 20;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mAttackOffsetHitBox = mWidth * 0.8f;

    SetSize(mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 3.7f, mWidth * 3.7f, "../Assets/Sprites/Mushroom/Mushroom.png", "../Assets/Sprites/Mushroom/Mushroom.json", 998);
    std::vector run = {18, 19, 20, 21, 22, 23, 24, 25, };
    mDrawAnimatedComponent->AddAnimation("run", run);

    std::vector idle = {13, 14, 15, 16, 47, 17, 46};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawAnimatedComponent->AddAnimation("attack", attack);

    std::vector stun = {26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43};
    mDrawAnimatedComponent->AddAnimation("stun", stun);

    std::vector hit = {44, 10, 11, 12, 45};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
}

void Mushroom::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mMushroomState != State::Attack) {
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->ResetAnimationTimer();
            }
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    if (mPlayerSpotted) {
        MovementAfterPlayerSpotted(deltaTime);
    }
    else {
        MovementBeforePlayerSpotted(deltaTime);
    }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    // Se morreu
    if (Died()) {
    }

    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }
}

void Mushroom::ReceiveHit(float damage, Vector2 knockBackDirection) {
    if (knockBackDirection.y == 1 && mMushroomState != State::Attack) {
        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("Boing/Boing.wav");
        }
        return;
    }

    mHealthPoints -= damage;
    mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() + knockBackDirection * mKnockBackSpeed);
    mKnockBackTimer = 0;
    mIsFlashing = true;
    mFlashTimer = 0;
    mPlayerSpotted = true;

    auto* blood = new ParticleSystem(mGame, 10, 170.0, 3.0, 0.07f);
    blood->SetPosition(GetPosition());
    blood->SetEmitDirection(knockBackDirection);
    blood->SetParticleSpeedScale(1);
    blood->SetParticleColor(SDL_Color{226, 90, 70, 255});
    blood->SetParticleGravity(true);

    auto* circleBlur = new Effect(mGame);
    circleBlur->SetDuration(0.3);
    circleBlur->SetSize((GetWidth() + GetHeight()) / 2 * 3.5f);
    circleBlur->SetEnemy(*this);
    circleBlur->SetColor(SDL_Color{226, 90, 70, 150});
    circleBlur->SetEffect(TargetEffect::Circle);

    if (IsOnScreen()) {
        mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
    }
}


void Mushroom::MovementBeforePlayerSpotted(float deltaTime) {
    mLookingAroundTimer += deltaTime;
    if (mLookingAroundTimer >= mLookingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mLookingAroundTimer = 0;
    }

    // Testa se spottou player
    Player* player = GetGame()->GetPlayer();
    if (Math::Abs(GetPosition().y - player->GetPosition().y) < 40 * mGame->GetScale()) { // Se está no mesmo nível verticalmente
        if (player->GetPosition().x < GetPosition().x && GetForward().x < 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
        else if (player->GetPosition().x > GetPosition().x && GetForward().x > 0 && Math::Abs(player->GetPosition().x - GetPosition().x) < mDistToSpotPlayer) {
            mPlayerSpotted = true;
        }
    }
}

void Mushroom::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mMushroomState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::WalkForward:
            WalkForward(deltaTime);
        break;

        case State::WalkAway:
            WalkAway(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;

        case State::Stun:
            Stun(deltaTime);
        break;
    }
}

void Mushroom::Stop(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mMushroomState = State::WalkForward;
    }
}

void Mushroom::WalkForward(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2, mRigidBodyComponent->GetVelocity().y));
    }

    if (Math::Abs(dist) < mDistToAttack) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2.5f, mJumpForce));
        mMushroomState = State::Attack;
        if (mDrawAnimatedComponent) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
    }
}

void Mushroom::WalkAway(float deltaTime) {
    mWalkAwayTimer += deltaTime;
    if (mWalkAwayTimer >= mWalkAwayDuration) {
        mWalkAwayTimer = 0;
        mMushroomState = State::Stop;
        return;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
    }
}

void Mushroom::Attack(float deltaTime) {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;

    mAttackTimer += deltaTime;
    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mMushroomState = State::Stun;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        if (mAttackTimer < 0.6f * mAttackDuration) {
            mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2.5f, mRigidBodyComponent->GetVelocity().y));
        }
        else {
            mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
        }
    }

    if (mAttackTimer > 0.2f * mAttackDuration && mAttackTimer < 0.4f * mAttackDuration) {
        if (GetRotation() == 0) {
            v1 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
        }
        else if (GetRotation() == Math::Pi) {
            v1 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
        }
    }
    else if (mAttackTimer > 0.5f * mAttackDuration && mAttackTimer < 0.9f * mAttackDuration) {
        if (GetRotation() == 0) {
            v1 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
        }
        else if (GetRotation() == Math::Pi) {
            v1 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
        }
    }
    else {
        v1 = Vector2(-mWidth / 2, -mHeight / 2);
        v2 = Vector2(mWidth / 2, -mHeight / 2);
        v3 = Vector2(mWidth / 2, mHeight / 2);
        v4 = Vector2(-mWidth / 2, mHeight / 2);
    }

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mColliderComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}

void Mushroom::Stun(float deltaTime) {
    mStumTimer += deltaTime;
    if (mStumTimer >= mStumDuration) {
        mStumTimer = 0;
        mMushroomState = State::WalkAway;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
}

void Mushroom::ManageAnimations() {
    if (mMushroomState == State::Attack) {
        mDrawAnimatedComponent->SetAnimation("attack");
        mDrawAnimatedComponent->SetAnimFPS(10.0f / mAttackDuration);
    }
    else if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(5.0f / mHitDuration);
    }
    else if (mMushroomState == State::WalkForward) {
        mDrawAnimatedComponent->SetAnimation("run");
        mDrawAnimatedComponent->SetAnimFPS(12);
    }
    else if (mMushroomState == State::WalkAway) {
        mDrawAnimatedComponent->SetAnimation("run");
        mDrawAnimatedComponent->SetAnimFPS(8);
    }
    else if (mMushroomState == State::Stun) {
        mDrawAnimatedComponent->SetAnimation("stun");
        mDrawAnimatedComponent->SetAnimFPS(12);
    }
    else if (mMushroomState == State::Stop) {
        mDrawAnimatedComponent->SetAnimation("idle");
        mDrawAnimatedComponent->SetAnimFPS(12);
    }
}

void Mushroom::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;
    mDistToAttack = mDistToAttack / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mAttackOffsetHitBox = mAttackOffsetHitBox / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 3.7f);
        mDrawAnimatedComponent->SetHeight(mWidth * 3.7f);
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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mColliderComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
