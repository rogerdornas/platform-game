//
// Created by roger on 21/09/2025.
//

#include "FlyingSpawnerEnemy.h"
#include "Effect.h"
#include "LittleBat.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

FlyingSpawnerEnemy::FlyingSpawnerEnemy(Game *game)
    :Enemy(game)
    ,mEnemyState(State::Fly)

    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mFlyingAroundDuration(1.0f)
    ,mFlyingAroundTimer(0.0f)
    ,mFlyingAroundMoveSpeed(100.0f * mGame->GetScale())

    ,mStopDuration(0.3f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)

    ,mPatrolRangeX(400.0f * mGame->GetScale())
    ,mPatrolRangeY(100.0f * mGame->GetScale())
    ,mHoverHeight(300.0f * mGame->GetScale())
    ,mTargetSet(false)
    ,mPatrolTargetDuration(1.0f)
    ,mPatrolTargetTimer(0.0f)

    ,mFlyDuration(0.6f)
    ,mFlyTimer(0.0f)

    ,mFlyAwayDuration(1.3f)
    ,mFlyAwayTimer(0.0f)

    ,mSmashAttachChargeDuration(0.8f)
    ,mSmashAttackChargeTimer(0.0f)
    ,mSmashAttackRecoveryDuration(0.6f)
    ,mSmashAttackRecoveryTimer(0.0f)

    ,mSpawnBatDuration(1.0f)
    ,mSpawnBatTimer(0.0f)
    ,mAlreadySpawnedBat(false)
    ,mMaxSpawnBat(2)
    ,mCountSpawnBat(0)
{
    mWidth = 96 * mGame->GetScale();
    mHeight = 96 * mGame->GetScale();
    mMoveSpeed = 400 * mGame->GetScale();
    mHealthPoints = 80;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 14;
    mKnockBackSpeed = 600.0f * mGame->GetScale();
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;
    mOriginalHeight = mHeight;
    mSmashHeight = mHeight * 1.7f;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/FlyingSpawnerEnemy/FlyingSpawnerEnemy.png",
                                                    "../Assets/Sprites/FlyingSpawnerEnemy/FlyingSpawnerEnemy.json",
                                                    mWidth * 2.0f, mHeight * 2.0f, 998);
    std::vector fly = {0, 1, 2, 3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("fly", fly);

    std::vector hit = {8, 9, 10, 11};
    mDrawComponent->AddAnimation("hit", hit);

    std::vector idle = {12, 13, 14, 15, 16, 17, 18, 19};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector smashStart = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42};
    mDrawComponent->AddAnimation("smashStart", smashStart);

    std::vector smashLoop = {28, 29, 30};
    mDrawComponent->AddAnimation("smashLoop", smashLoop);

    std::vector smashEnd = {20, 21, 22, 23, 24, 25, 26, 27};
    mDrawComponent->AddAnimation("smashEnd", smashEnd);

    std::vector spawn = {43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56};
    mDrawComponent->AddAnimation("spawn", spawn);

    mDrawComponent->SetAnimation("fly");
    mDrawComponent->SetAnimFPS(12.0f);
}

void FlyingSpawnerEnemy::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mEnemyState != State::SmashAttackCharge &&
            mEnemyState != State::SmashAttack &&
            mEnemyState != State::SmashAttackRecovery &&
            mEnemyState != State::SpawnBat)
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
        if (mPlayerSpotted) {
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted();
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

void FlyingSpawnerEnemy::ResolveGroundCollision() {
    Vector2 collisionNormal;
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    if (mEnemyState == State::SmashAttack && collisionNormal == Vector2::NegUnitY) {
                        // if (mDrawAnimatedComponent) {
                        //     mDrawAnimatedComponent->ResetAnimationTimer();
                        // }
                        mEnemyState = State::SmashAttackRecovery;
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (mEnemyState == State::SmashAttack && collisionNormal == Vector2::NegUnitY) {
                        ReceiveHit(10, Vector2::NegUnitY);
                        // if (mDrawAnimatedComponent) {
                        //     mDrawAnimatedComponent->ResetAnimationTimer();
                        // }
                        mEnemyState = State::SmashAttackRecovery;
                    }
                    // Colidiu bot
                    if (collisionNormal == Vector2::UnitY) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionNormal == Vector2::NegUnitX) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionNormal == Vector2::UnitX) {
                        ReceiveHit(10, Vector2::UnitX);
                    }
                    mKnockBackTimer = 0;
                }
            }
        }
    }
}


void FlyingSpawnerEnemy::MovementBeforePlayerSpotted() {
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

void FlyingSpawnerEnemy::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mEnemyState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Fly:
            Fly(deltaTime);
        break;

        case State::FlyAway:
            FlyAway(deltaTime);
        break;

        case State::SmashAttackCharge:
            SmashAttackCharge(deltaTime);
        break;

        case State::SmashAttack:
            SmashAttack(deltaTime);
        break;

        case State::SmashAttackRecovery:
            SmashAttackRecovery(deltaTime);
        break;

        case State::SpawnBat:
            SpawnBat(deltaTime);
        break;
    }
}

void FlyingSpawnerEnemy::Stop(float deltaTime) {
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
        mEnemyState = State::Fly;
    }
}

void FlyingSpawnerEnemy::Fly(float deltaTime) {
    mFlyTimer += deltaTime;
    mPatrolTargetTimer += deltaTime;
    if (mFlyTimer >= mFlyDuration) {
        mFlyTimer = 0;
        mTargetSet = false; // reseta alvo da próxima patrulha
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
        if (Random::GetFloat() < 0.5) {
            mEnemyState = State::SmashAttackCharge;
        }
        else {
            mEnemyState = State::SpawnBat;
        }
        return;
    }
    Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

    // Definir um novo alvo lateral se ainda não tiver
    if (!mTargetSet) {
        float offsetX = Random::GetFloatRange(-mPatrolRangeX / 2.0f, mPatrolRangeX / 2.0f);
        mCurrentTarget.x = playerPos.x + offsetX;
        float offsetY = Random::GetFloatRange(-mPatrolRangeY / 2.0f, mPatrolRangeY / 2.0f) + mHoverHeight;
        mCurrentTarget.y = playerPos.y - offsetY;
        mTargetSet = true;
    }

    // Movimento em direção ao alvo atual
    Vector2 direction = mCurrentTarget - GetPosition();
    float distance = direction.Length();

    if (mPatrolTargetTimer >= mPatrolTargetDuration || distance < 5.0f * mGame->GetScale()) {
        // Quando chegar perto do alvo ou passar o tempo, escolher novo ponto
        mPatrolTargetTimer = 0;
        mTargetSet = false;
    }
    else {
        if (direction.Length() > 0) {
            direction.Normalize();
        }
        float angle = Math::Atan2(direction.y, direction.x);
        // Ajustar para intervalo [0, 2*pi)
        if (angle < 0) {
            angle += 2 * Math::Pi;
        }

        SetRotation(angle);

        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
        }
    }
}

void FlyingSpawnerEnemy::FlyAway(float deltaTime) {
    mFlyAwayTimer += deltaTime;
    if (mFlyAwayTimer >= mFlyAwayDuration || GetPosition() == mCurrentTarget) {
        mFlyAwayTimer = 0;
        mEnemyState = State::Stop;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
    }
}

void FlyingSpawnerEnemy::SmashAttackCharge(float deltaTime) {
    mSmashAttackChargeTimer += deltaTime;
    if (mSmashAttackChargeTimer >= mSmashAttachChargeDuration) {
        mSmashAttackChargeTimer = 0;
        // if (mDrawAnimatedComponent) {
        //     mDrawAnimatedComponent->ResetAnimationTimer();
        // }
        mEnemyState = State::SmashAttack;
    }

    mHeight = mSmashHeight;

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

    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void FlyingSpawnerEnemy::SmashAttack(float deltaTime) {
    mEnemyCollision = false;
    mRigidBodyComponent->SetVelocity(Vector2::UnitY * mMoveSpeed * 4.0f);
}

void FlyingSpawnerEnemy::SmashAttackRecovery(float deltaTime) {
    mSmashAttackRecoveryTimer += deltaTime;
    mEnemyCollision = true;
    if (mSmashAttackRecoveryTimer >= mSmashAttackRecoveryDuration) {
        mSmashAttackRecoveryTimer = 0;

        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

        float offsetX = Random::GetFloatRange(-mPatrolRangeX / 2.0f, mPatrolRangeX / 2.0f);
        mCurrentTarget.x = playerPos.x + offsetX;
        float offsetY = Random::GetFloatRange(-mPatrolRangeY / 2.0f, mPatrolRangeY / 2.0f) + mHoverHeight;
        mCurrentTarget.y = playerPos.y - offsetY;

        Vector2 direction = mCurrentTarget - GetPosition();
        float angle = Math::Atan2(direction.y, direction.x);
        // Ajustar para intervalo [0, 2*pi)
        if (angle < 0) {
            angle += 2 * Math::Pi;
        }

        SetRotation(angle);

        mEnemyState = State::FlyAway;
    }

    mHeight = mOriginalHeight;

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

    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
    }

    mRigidBodyComponent->SetVelocity(Vector2::Zero);
}

void FlyingSpawnerEnemy::SpawnBat(float deltaTime) {
    mSpawnBatTimer += deltaTime;
    if (mSpawnBatTimer >= mSpawnBatDuration) {
        mSpawnBatTimer = 0;
        mAlreadySpawnedBat = false;
        mCountSpawnBat++;
    }

    if (mCountSpawnBat >= mMaxSpawnBat) {
        mCountSpawnBat = 0;
        mEnemyState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (!mAlreadySpawnedBat) {
        if (mSpawnBatTimer >= mSpawnBatDuration * 0.83f) {
            auto* littleBat = new LittleBat(mGame);
            littleBat->SetPosition(GetPosition() + Vector2(mWidth * 0.8f * GetForward().x, 30 * mGame->GetScale()));
            littleBat->SetSpottedPlayer(true);
            mAlreadySpawnedBat = true;
        }
    }
}

void FlyingSpawnerEnemy::ManageAnimations() {
    mDrawComponent->SetAnimFPS(12.0f);
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        SetScale(Vector2(-1,1));
        // mDrawAnimatedComponent->UseFlip(true);
        // mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
    }
    else {
        SetScale(Vector2(1,1));
        // mDrawAnimatedComponent->UseFlip(false);
    }

    if (mEnemyState == State::SmashAttackCharge) {
        mDrawComponent->SetAnimation("smashStart");
        mDrawComponent->SetAnimFPS(12.0f / mSmashAttachChargeDuration);
    }
    else if (mEnemyState == State::SmashAttack) {
        mDrawComponent->SetAnimation("smashLoop");
    }
    else if (mEnemyState == State::SmashAttackRecovery) {
        mDrawComponent->SetAnimation("smashEnd");
        mDrawComponent->SetAnimFPS(8.0f / mSmashAttackRecoveryDuration);
    }
    else if (mEnemyState == State::SpawnBat) {
        mDrawComponent->SetAnimation("spawn");
        mDrawComponent->SetAnimFPS(14.0f / mSpawnBatDuration);
    }
    else if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
        mDrawComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else if (mEnemyState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
    }
    else if (mEnemyState == State::Fly || mEnemyState == State::FlyAway) {
        mDrawComponent->SetAnimation("fly");
    }
}

void FlyingSpawnerEnemy::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mFlyingAroundMoveSpeed = mFlyingAroundMoveSpeed / oldScale * newScale;
    mHoverHeight = mHoverHeight / oldScale * newScale;
    mPatrolRangeX = mPatrolRangeX / oldScale * newScale;
    mPatrolRangeY = mPatrolRangeY / oldScale * newScale;
    mOriginalHeight = mOriginalHeight / oldScale * newScale;
    mSmashHeight = mSmashHeight / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 2.0f);
    //     mDrawAnimatedComponent->SetHeight(mOriginalHeight * 2.0f);
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
