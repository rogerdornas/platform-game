//
// Created by roger on 29/07/2025.
//

#include "FlyingGolem.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

FlyingGolem::FlyingGolem(Game *game)
    :Enemy(game)
    ,mFlyingGolemState(State::FlyingAround)

    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mFlyingAroundDuration(1.0f)
    ,mFlyingAroundTimer(mFlyingAroundDuration)
    ,mFlyingAroundMoveSpeed(100.0f * mGame->GetScale())

    ,mStopDuration(1.3f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mAttackDuration(0.7f)
    ,mAttackTimer(0.0f)
    ,mDistToAttack(400 * mGame->GetScale())
    ,mAttackDirectionRight(true)

    ,mDistToTeleport(1100 * mGame->GetScale())
    ,mTeleportDuration(0.6f)
    ,mTeleportInTimer(0.0f)
    ,mTeleportOutTimer(0.0f)
    ,mTeleportTargetPosition(Vector2::Zero)
    ,mTeleportHoverHeight(250 * mGame->GetScale())
    ,mTeleportRangeTargetX(400* mGame->GetScale())
{
    mWidth = 100 * mGame->GetScale();
    mHeight = 100 * mGame->GetScale();
    mMoveSpeed = 420 * mGame->GetScale();
    mHealthPoints = 100;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 20;
    mKnockBackSpeed = 500.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;
    mIdleWidth = mWidth;
    mAttackSpriteWidth = mWidth * 1.5f;
    mAttackOffsetHitBox = mWidth * 0.7f;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/FlyingGolem2/FlyingGolem.png",
                                                    "../Assets/Sprites/FlyingGolem2/FlyingGolem.json",
                                                    mWidth * 1.7f * 2.0f, mWidth * 1.7f, 998);
    std::vector idle = {22, 23, 24, 25, 26, 27, 28, 29};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector hit = {19, 20, 21, 37};
    mDrawComponent->AddAnimation("hit", hit);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mDrawComponent->AddAnimation("attack", attack);

    std::vector fly = {11, 12, 13, 14, 15, 16, 17, 18};
    mDrawComponent->AddAnimation("fly", fly);

    std::vector teleportIn = {38, 30, 31, 32, 33, 34, 35, 36};
    mDrawComponent->AddAnimation("teleportIn", teleportIn);

    std::vector teleportOut = {46, 45, 44, 43, 42, 41, 40, 39};
    mDrawComponent->AddAnimation("teleportOut", teleportOut);

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(10.0f);
}

void FlyingGolem::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mFlyingGolemState != State::Attack &&
            mFlyingGolemState != State::TeleportIn &&
            mFlyingGolemState != State::TeleportOut)
        {
            // if (mDrawAnimatedComponent) {
            //     mDrawAnimatedComponent->ResetAnimationTimer();
            // }
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();
    ManageFreezing(deltaTime);

    if (!mIsFrozen) {
        MovementAfterPlayerSpotted(deltaTime);

        // Teleporta se estiver longe
        if (mFlyingGolemState != State::FlyingAround &&
            mFlyingGolemState != State::TeleportIn &&
            mFlyingGolemState != State::TeleportOut)
        {
            float dist = (GetPosition() - mGame->GetPlayer()->GetPosition()).Length();
            if (dist >= mDistToTeleport) {
                mFlyingGolemState = State::TeleportIn;
                // if (mDrawAnimatedComponent) {
                //     mDrawAnimatedComponent->ResetAnimationTimer();
                // }
            }
        }
    }

    // Se morreu
    if (Died()) {
    }

    if (!mIsFrozen) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }
}

void FlyingGolem::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mFlyingGolemState) {
        case State::FlyingAround:
            FlyingAround(deltaTime);
        break;

        case State::Stop:
            Stop(deltaTime);
        break;

        case State::FlyForward:
            FlyForward(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;

        case State::TeleportIn:
            TeleportIn(deltaTime);
        break;

        case State::TeleportOut:
            TeleportOut(deltaTime);
        break;
    }
}

void FlyingGolem::MovementBeforePlayerSpotted() {
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

void FlyingGolem::FlyingAround(float deltaTime) {
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

    if (mPlayerSpotted) {
        mFlyingGolemState = State::Stop;
    }
}

void FlyingGolem::Stop(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mFlyingGolemState = State::FlyForward;
    }
}

void FlyingGolem::FlyForward(float deltaTime) {
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

    float dist = (GetPosition() - player->GetPosition()).Length();
    if (dist <= mDistToAttack) {
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mAttackDirectionRight = false;
        }
        else {
            mAttackDirectionRight = true;
        }
        mFlyingGolemState = State::Attack;
    }
}

void FlyingGolem::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed * 1.7);
    }

    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mFlyingGolemState = State::TeleportIn;
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
    }

    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;

    if (mAttackTimer > 0.35f * mAttackDuration && mAttackTimer < 0.82 * mAttackDuration) {
        mWidth = mAttackSpriteWidth;

        if (mAttackDirectionRight) {
            v1 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
        }
        else {
            v1 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 - mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 - mAttackOffsetHitBox, mHeight / 2);
        }
    }
    else {
        mWidth = mIdleWidth;

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

    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
    }
}

void FlyingGolem::TeleportIn(float deltaTime) {
    mTeleportInTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));

    if (mTeleportInTimer >= mTeleportDuration) {
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        float offsetX = Random::GetFloatRange(-mTeleportRangeTargetX / 2.0f, mTeleportRangeTargetX / 2.0f);
        mTeleportTargetPosition.x = playerPos.x + offsetX;
        mTeleportTargetPosition.y = playerPos.y - mTeleportHoverHeight;
        SetPosition(mTeleportTargetPosition);

        mTeleportInTimer = 0;
        mFlyingGolemState = State::TeleportOut;
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
        return;
    }

    if (mTeleportInTimer > 0.5f * mTeleportDuration) {
        mColliderComponent->SetActive(false);
    }
}

void FlyingGolem::TeleportOut(float deltaTime) {
    mTeleportOutTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));

    if (mTeleportOutTimer >= mTeleportDuration) {
        mTeleportOutTimer = 0;
        mFlyingGolemState = State::Stop;
        return;
    }

    if (mTeleportOutTimer > 0.1f * mTeleportDuration) {
        mColliderComponent->SetActive(true);
    }
}

void FlyingGolem::ManageAnimations() {
    mDrawComponent->SetAnimFPS(10.0f);
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        SetScale(Vector2(-1,1));
        // mDrawAnimatedComponent->UseFlip(true);
        // mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
    }
    else {
        SetScale(Vector2(1,1));
        // mDrawAnimatedComponent->UseFlip(false);
    }

    if (mFlyingGolemState == State::Attack) {
        mDrawComponent->SetAnimation("attack");
        mDrawComponent->SetAnimFPS(11.0f / mAttackDuration);
    }
    else if (mFlyingGolemState == State::TeleportIn) {
        mDrawComponent->SetAnimation("teleportIn");
        mDrawComponent->SetAnimFPS(8.0f / mTeleportDuration);
    }
    else if (mFlyingGolemState == State::TeleportOut) {
        mDrawComponent->SetAnimation("teleportOut");
        mDrawComponent->SetAnimFPS(8.0f / mTeleportDuration);
    }
    else if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
        mDrawComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else if (mFlyingGolemState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
    }
    else if (mFlyingGolemState == State::FlyForward) {
        mDrawComponent->SetAnimation("fly");
        mDrawComponent->SetAnimFPS(15.0f);
    }
    else if (mFlyingGolemState == State::FlyingAround) {
        mDrawComponent->SetAnimation("fly");
    }
}

void FlyingGolem::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mFlyingAroundMoveSpeed = mFlyingAroundMoveSpeed / oldScale * newScale;
    mDistToAttack = mDistToAttack / oldScale * newScale;
    mIdleWidth = mIdleWidth / oldScale * newScale;
    mAttackSpriteWidth = mAttackSpriteWidth / oldScale * newScale;
    mAttackOffsetHitBox = mAttackOffsetHitBox / oldScale * newScale;
    mDistToTeleport = mDistToTeleport / oldScale * newScale;
    mTeleportTargetPosition.x = mTeleportTargetPosition.x / oldScale * newScale;
    mTeleportTargetPosition.y = mTeleportTargetPosition.y / oldScale * newScale;
    mTeleportHoverHeight = mTeleportHoverHeight / oldScale * newScale;
    mTeleportRangeTargetX = mTeleportRangeTargetX / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mIdleWidth * 1.7f * 2.0f);
    //     mDrawAnimatedComponent->SetHeight(mIdleWidth * 1.7f);
    // }

    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;

    if (mWidth == mAttackSpriteWidth) {
        if (mAttackDirectionRight) {
            v1 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v2 = Vector2(mWidth / 2 + mAttackOffsetHitBox, -mHeight / 2);
            v3 = Vector2(mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
            v4 = Vector2(-mWidth / 2 + mAttackOffsetHitBox, mHeight / 2);
        }
        else {
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

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
