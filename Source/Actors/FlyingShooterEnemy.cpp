//
// Created by roger on 09/06/2025.
//

#include "FlyingShooterEnemy.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Projectile.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

FlyingShooterEnemy::FlyingShooterEnemy(Game* game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 5.0f)
    ,mState(State::Fly)
    ,mPatrolRangeX(400.0f * mGame->GetScale())
    ,mPatrolRangeY(100.0f * mGame->GetScale())
    ,mHoverHeight(300.0f * mGame->GetScale())
    ,mTargetSet(false)
    ,mPatrolTargetDuration(1.0f)
    ,mPatrolTargetTimer(0.0f)
    ,mFlyDuration(1.8f)
    ,mFlyTimer(0.0f)
    ,mShootDuration(1.2f)
    ,mShootTimer(0.0f)
    ,mShot(false)
    ,mProjectileWidth(50 * mGame->GetScale())
    ,mProjectileHeight(55 * mGame->GetScale())
    ,mProjectileSpeed(800 * mGame->GetScale())
{
    mMoneyDrop = 6;
    mMoveSpeed = 300 * mGame->GetScale();
    mKnockBackSpeed = 800.0f * mGame->GetScale();
    mKnockBackDuration = 0.2f;
    mKnockBackTimer = mKnockBackDuration;
    mDistToSpotPlayer = 400 * mGame->GetScale();
    mFlyingAroundDuration = 1.0f;
    mFlyingAroundTimer = mFlyingAroundDuration;
    mFlyingAroundMoveSpeed = 100.0f * mGame->GetScale();

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.0f, mHeight * 2.0f, "../Assets/Sprites/Beetle/Beetle.png", "../Assets/Sprites/Beetle/Beetle.json", 999);
    std::vector fly = {0, 1, 2, 3};
    mDrawAnimatedComponent->AddAnimation("fly", fly);

    std::vector hit = {4};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("fly");
    mDrawAnimatedComponent->SetAnimFPS(8.0f);
}

void FlyingShooterEnemy::OnUpdate(float deltaTime) {
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

void FlyingShooterEnemy::MovementBeforePlayerSpotted() {
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

void FlyingShooterEnemy::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mState) {
        case State::Fly:
            Fly(deltaTime);
        break;

        case State::Shoot:
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
        mState = State::Shoot;
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

void FlyingShooterEnemy::Shoot(float deltaTime) {
    mShootTimer += deltaTime;
    if (mShootTimer >= mShootDuration) {
        mShootTimer = 0;
        mShot = false;
        mState = State::Fly;
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
        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused && p->GetProjectileType() == Projectile::ProjectileType::Acid) {
                p->SetState(ActorState::Active);
                p->SetRotation(Math::Atan2(direction.y, direction.x));
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed);
                p->SetDamage(10);
                if (GetRotation() == 0) {
                    p->SetPosition(GetPosition() + Vector2(mWidth / 2,0));
                }
                else if (GetRotation() == Math::Pi) {
                    p->SetPosition(GetPosition() - Vector2(mWidth / 2,0));
                }
                break;
            }
        }
        mShot = true;
    }

    // Controla rotação
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }
}

void FlyingShooterEnemy::ManageAnimations() {
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
