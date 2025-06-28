//
// Created by roger on 28/06/2025.
//

#include "Golem.h"
#include "Effect.h"
#include "Lever.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Actors/FireBall.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

Golem::Golem(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 10)
    ,mGolemState(State::Stop)

    ,mIsRunning(false)
    ,mGravity(3000 * mGame->GetScale())
    ,mIsInvulnerable(false)
    ,mAlreadySpawnedCrystal(false)
    ,mCrystalWidth(64)
    ,mCrystalHeight(64)

    ,mStopDuration(0.8f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mRunAwayDuration(0.8f)
    ,mRunAwayTimer(0.0f)

    ,mPunchDuration(0.4f)
    ,mPunchTimer(0.0f)
    ,mDistToPunch(200 * mGame->GetScale())
    ,mIdleWidth(mWidth)
    ,mPunchSpriteWidth(mWidth * 1.5f)
    ,mPunchOffsetHitBox(mWidth * 0.8f)
    ,mPunchDirectionRight(true)

    ,mFireballDuration(1.0f)
    ,mFireballTimer(0.0f)
    ,mAlreadyFireBalled(false)
    ,mFireballWidth(100 * mGame->GetScale())
    ,mFireBallHeight(100 * mGame->GetScale())
    ,mFireballSpeed(1400 * mGame->GetScale())
    ,mFireballDamage(20)
{
    mMoneyDrop = 200;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.7f * 1.875f, mWidth * 1.7f, "../Assets/Sprites/Golem/Golem.png", "../Assets/Sprites/Golem/Golem.json", 999);
    std::vector idle = {14, 15, 16, 17, 18, 19, 20, 21};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector walk = {22, 23, 24, 25, 26, 27 ,28, 29, 30, 31};
    mDrawAnimatedComponent->AddAnimation("walk", walk);

    std::vector punch = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawAnimatedComponent->AddAnimation("punch", punch);

    std::vector hit = {10, 11, 12, 13};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);

}

void Golem::OnUpdate(float deltaTime) {
    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mGolemState != State::Punch) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y
                                             + mGravity * deltaTime));

    if (mPlayerSpotted) {
        if (!mGame->GetBossMusicHandle().IsValid()) {
            mGame->StartBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav", true));
        }
        MovementAfterPlayerSpotted(deltaTime);
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Se morreu
    if (Died()) {
        // mMothState = State::Dying;
        TriggerBossDefeat();
    }

    ControlSpawCrystal();

    ManageAnimations();

    SDL_Log("%f", mHealthPoints);
}

void Golem::MovementBeforePlayerSpotted() {

}

void Golem::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mGolemState) {
        case State::Stop:
            Stop(deltaTime);
            break;

        case State::RunAway:
            RunAway(deltaTime);
            break;

        case State::RunForward:
            RunForward(deltaTime);
            break;

        case State::Punch:
            Punch(deltaTime);
            break;

        case State::Fireball:
            Fireball(deltaTime);
            break;
    }
}

void Golem::Stop(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        if (Random::GetFloat() < 0.6) {
            mGolemState = State::RunForward;
        }
        else {
            mGolemState = State::Fireball;
        }
    }
}

void Golem::RunAway(float deltaTime) {
    mIsRunning = true;
    mRunAwayTimer += deltaTime;
    if (mRunAwayTimer >= mRunAwayDuration) {
        mRunAwayTimer = 0;
        mGolemState = State::Stop;
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
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 3, mRigidBodyComponent->GetVelocity().y));
}

void Golem::RunForward(float deltaTime) {
    mIsRunning = true;
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 3.5, mRigidBodyComponent->GetVelocity().y));

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (Math::Abs(dist) < mDistToPunch) {
        mDrawAnimatedComponent->ResetAnimationTimer();
        if (GetRotation() == 0) {
            mPunchDirectionRight = true;
        }
        else {
            mPunchDirectionRight = false;
        }
        mGolemState = State::Punch;
        return;
    }

}

void Golem::Punch(float deltaTime) {
    mPunchTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    if (mPunchDirectionRight) {
        SetRotation(0);
    }
    else {
        SetRotation(Math::Pi);
    }

    if (mPunchTimer >= mPunchDuration) {
        mPunchTimer = 0;
        mGolemState = State::Stop;
    }

    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;

    if (mPunchTimer > 0.5f * mPunchDuration && mPunchTimer < 0.8f * mPunchDuration) {
        mWidth = mPunchSpriteWidth;

        v1 = Vector2(-mWidth / 2 + mPunchOffsetHitBox * GetForward().x, -mHeight / 2);
        v2 = Vector2(mWidth / 2 + mPunchOffsetHitBox * GetForward().x, -mHeight / 2);
        v3 = Vector2(mWidth / 2 + mPunchOffsetHitBox * GetForward().x, mHeight / 2);
        v4 = Vector2(-mWidth / 2 + mPunchOffsetHitBox * GetForward().x, mHeight / 2);
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

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}


void Golem::Fireball(float deltaTime) {
    mFireballTimer += deltaTime;
    if (mFireballTimer >= mFireballDuration) {
        mFireballTimer = 0;
        mAlreadyFireBalled = false;
        if (Random::GetFloat() < 0.0) {
            mGolemState = State::Fireball;
        }
        else {
            mGolemState = State::Stop;
        }
    }

    if (!mAlreadyFireBalled) {
        std::vector<FireBall* > fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetWidth(mFireballWidth);
                f->SetHeight(mFireBallHeight);
                f->SetSpeed(mFireballSpeed);
                f->SetDamage(mFireballDamage);
                f->SetIsFromEnemy();
                f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                break;
            }
        }
        mAlreadyFireBalled = true;
    }

}

void Golem::ControlSpawCrystal() {
    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.7f * mMaxHealthPoints && mHealthPoints <= 0.8f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.7f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }

    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.5f * mMaxHealthPoints && mHealthPoints <= 0.6f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.4f * mMaxHealthPoints && mHealthPoints <= 0.5f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }

    if (!mAlreadySpawnedCrystal) {
        if (mHealthPoints > 0.3f * mMaxHealthPoints && mHealthPoints <= 0.4f * mMaxHealthPoints) {
            Vector2 position(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x),
                            mArenaMaxPos.y);
            SpawCrystal(position);
            mAlreadySpawnedCrystal = true;
        }
    }

    if (mHealthPoints > 0.2f * mMaxHealthPoints && mHealthPoints <= 0.3f * mMaxHealthPoints) {
        mAlreadySpawnedCrystal = false;
    }
}


void Golem::SpawCrystal(Vector2 position) {
    mIsInvulnerable = true;
    auto* crystal = new Lever(mGame, mCrystalWidth, mCrystalHeight, Lever::LeverType::Crystal);
    crystal->SetPosition(position);
    crystal->SetTarget("Enemy");
    crystal->SetEvent("GolemVulnerable");
    crystal->SetEnemiesIds(std::vector<int>{mId});
}


void Golem::TriggerBossDefeat() {
    SetState(ActorState::Destroy);
    mGame->GetCamera()->StartCameraShake(0.5, mCameraShakeStrength);

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

    mGame->StopBossMusic();
}

void Golem::ReceiveHit(float damage, Vector2 knockBackDirection) {
    if (knockBackDirection.y == 1) {
        mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
        for (int i = 0; i < 3; i++) {
            auto* sparkEffect = new Effect(mGame);
            sparkEffect->SetDuration(0.1f);
            sparkEffect->SetPosition(Vector2(mGame->GetPlayer()->GetPosition().x, GetPosition().y - mHeight * 0.3f));
            sparkEffect->SetEffect(TargetEffect::SwordHit);
        }
        return;
    }

    if (mIsInvulnerable) {
        if (knockBackDirection.y == -1) {
            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                sparkEffect->SetPosition(Vector2(mGame->GetPlayer()->GetPosition().x, GetPosition().y + mHeight * 0.3f));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.x == 1) {
            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                float positionY = mGame->GetPlayer()->GetPosition().y;
                if (positionY < GetPosition().y - mHeight * 0.3f) {
                    positionY = GetPosition().y - mHeight * 0.3f;
                }
                sparkEffect->SetPosition(Vector2(GetPosition().x - mWidth * 0.3f, positionY));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.x == -1) {
            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                float positionY = mGame->GetPlayer()->GetPosition().y;
                if (positionY < GetPosition().y - mHeight * 0.3f) {
                    positionY = GetPosition().y - mHeight * 0.3f;
                }
                sparkEffect->SetPosition(Vector2(GetPosition().x + mWidth * 0.3f, positionY));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
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

    mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
}


void Golem::ManageAnimations() {
    if (mGolemState == State::Punch) {
        mDrawAnimatedComponent->SetAnimation("punch");
        mDrawAnimatedComponent->SetAnimFPS(10.0f / mPunchDuration);
    }
    else if (mGolemState == State::RunForward ||
             mGolemState == State::RunAway) {
        mDrawAnimatedComponent->SetAnimation("walk");
        mDrawAnimatedComponent->SetAnimFPS(30.0f);
    }
    else if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else {
        mDrawAnimatedComponent->SetAnimation("idle");
        mDrawAnimatedComponent->SetAnimFPS(10.0f);
    }
}

void Golem::ChangeResolution(float oldScale, float newScale) {

}
