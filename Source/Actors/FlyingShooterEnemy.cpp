//
// Created by roger on 09/06/2025.
//

#include "FlyingShooterEnemy.h"
#include "FlyingEnemySimple.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Projectile.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

FlyingShooterEnemy::FlyingShooterEnemy(Game *game, float width, float height, float movespeed, float healthpoints)
    : FlyingEnemySimple(game, width, height, movespeed, healthpoints)
    ,mState(FlyingShooterEnemy::State::Fly)
    ,mHoverHeight(300.0f * mGame->GetScale())
    ,mPatrolRangeX(400.0f * mGame->GetScale())
    ,mPatrolRangeY(100.0f * mGame->GetScale())
    ,mTargetSet(false)
    ,mPatrolTargetDuration(1.0f)
    ,mPatrolTargetTimer(0.0f)
    ,mFlyDuration(1.8f)
    ,mFlyTimer(0.0f)
    ,mShootDuration(1.2f)
    ,mShootTimer(0.0f)
    ,mShot(false)
    ,mProjectileWidth(50 * mGame->GetScale())
    ,mProjectileHeight(50 * mGame->GetScale())
    ,mProjectileSpeed(800 * mGame->GetScale())
{
    mMoveSpeed = 300 * mGame->GetScale();
    mKnockBackSpeed = 800.0f * mGame->GetScale();
}

void FlyingShooterEnemy::MovementAfterPlayerSpotted(float deltaTime)
{
    switch (mState) {
        case FlyingShooterEnemy::State::Fly:
            Fly(deltaTime);
        break;

        case FlyingShooterEnemy::State::Shoot:
            Shoot(deltaTime);
        break;
    }
}

void FlyingShooterEnemy::Fly(float deltaTime) {
    mFlyTimer += deltaTime;
    mPatrolTargetTimer += deltaTime;
    if (mFlyTimer >= mFlyDuration) {
        mFlyTimer = 0;
        mTargetSet = false; // reseta alvo da próxima patrulha
        mState = FlyingShooterEnemy::State::Shoot;
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

    if (mPatrolTargetTimer >= mPatrolTargetDuration || distance < 5.0f) {
        // Quando chegar perto do alvo ou passar o tempo, escolher novo ponto
        mPatrolTargetTimer = 0;
        mTargetSet = false;
    }
    else {
        if (direction.Length() > 0) {
            direction.Normalize();
        }
        SetRotation(Math::Atan2(direction.y, direction.x));
        if (mKnockBackTimer >= mKnockBackDuration) {
            mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
        }
    }
}

void FlyingShooterEnemy::Shoot(float deltaTime) {
    mShootTimer += deltaTime;
    if (mShootTimer >= mShootDuration) {
        mShootTimer = 0;
        mShot = false;
        mState = FlyingShooterEnemy::State::Fly;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (!mShot && mShootTimer >= mShootDuration / 2) {
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        Vector2 direction = playerPos - GetPosition();
        if (direction.Length() > 0) {
            direction.Normalize();
        }
        std::vector<Projectile *> projectiles = mGame->GetProjectiles();
        for (Projectile *p: projectiles) {
            if (p->GetState() == ActorState::Paused) {
                p->SetState(ActorState::Active);
                p->SetRotation(Math::Atan2(direction.y, direction.x));
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed);
                p->SetDamage(10);
                p->SetPosition(GetPosition());
                break;
            }
        }
        mShot = true;
    }
}

void FlyingShooterEnemy::ChangeResolution(float oldScale, float newScale) {
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
    mProjectileWidth = mProjectileWidth / oldScale * newScale;
    mProjectileHeight = mProjectileHeight / oldScale * newScale;
    mProjectileSpeed = mProjectileSpeed / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawSpriteComponent->SetWidth(mWidth * 1.28f);
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

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
