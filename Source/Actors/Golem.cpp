//
// Created by roger on 28/06/2025.
//

#include "Golem.h"
#include "Effect.h"
#include "Lever.h"
#include "ParticleSystem.h"
#include "Skill.h"
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
    ,mPunchProbability(0.5f)

    ,mIsInvulnerable(false)
    ,mAlreadySpawnedCrystal(false)
    ,mCrystalWidth(108)
    ,mCrystalHeight(72)

    ,mStopDuration(1.2f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mRunAwayDuration(0.8f)
    ,mRunAwayTimer(0.0f)
    ,mMoveSpeedIncrease(1.0f)

    ,mPunchDuration(0.4f)
    ,mPunchTimer(0.0f)
    ,mDistToPunch(200 * mGame->GetScale())
    ,mIdleWidth(mWidth)
    ,mPunchSpriteWidth(mWidth * 1.5f)
    ,mPunchOffsetHitBox(mWidth * 0.9f)
    ,mPunchDirectionRight(true)

    ,mFireballDuration(1.0f)
    ,mFireballTimer(0.0f)
    ,mAlreadyFireBalled(false)
    ,mFireballWidth(100 * mGame->GetScale())
    ,mFireBallHeight(100 * mGame->GetScale())
    ,mFireballSpeed(1400 * mGame->GetScale())
    ,mFireballDamage(15)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
{
    mMoneyDrop = 50;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.8f * 1.875f, mWidth * 1.8f, "../Assets/Sprites/Golem2/Golem.png", "../Assets/Sprites/Golem2/Golem.json", 999);
    std::vector idle = {54, 22, 23, 24, 55, 25, 58, 26};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector idleInvulnerable = {56, 27, 28, 29, 57, 30, 59, 31};
    mDrawAnimatedComponent->AddAnimation("idleInvulnerable", idleInvulnerable);

    std::vector walk = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41};
    mDrawAnimatedComponent->AddAnimation("walk", walk);

    std::vector walkInvulnerable = {42, 43, 44, 45, 46, 47, 48, 49, 50, 51};
    mDrawAnimatedComponent->AddAnimation("walkInvulnerable", walkInvulnerable);

    std::vector punch = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    mDrawAnimatedComponent->AddAnimation("punch", punch);

    std::vector punchInvulnerable = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    mDrawAnimatedComponent->AddAnimation("punchInvulnerable", punchInvulnerable);

    std::vector hit = {52, 20, 21, 53};
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

    // Controla sons de passos
    if (mGolemState == State::RunForward) {
        mRunningSoundIntervalTimer += deltaTime;
        if (mRunningSoundIntervalTimer >= mRunningSoundIntervalDuration) {
            mRunningSoundIntervalTimer -= mRunningSoundIntervalDuration;
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("GolemSteps/GolemSteps.wav");
            }
        }
    }

    ManageAnimations();

    if (mHealthPoints <= 0.65f * mMaxHealthPoints) {
        mStopDuration = 1.0f;
        mFireballDuration = 0.7f;
        mMoveSpeedIncrease = 2;
        mRunningSoundIntervalDuration = 0.2;
    }
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
        if (Random::GetFloat() < mPunchProbability) {
            mGolemState = State::RunForward;
        }
        else {
            mGolemState = State::Fireball;
        }

        // Controla probabilidade de soco para não ficar spamando
        if (mGolemState == State::RunForward) {
            mPunchProbability -= 0.1;
        }
        else {
            mPunchProbability += 0.1;
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
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
}

void Golem::RunForward(float deltaTime) {
    mIsRunning = true;
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * mMoveSpeedIncrease, mRigidBodyComponent->GetVelocity().y));

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

    // Player ganha bola de fogo
    auto* skill = new Skill(mGame, Skill::SkillType::FireBall);
    skill->SetPosition(GetPosition());

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
        if (IsOnScreen()) {
            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
        }
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
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                sparkEffect->SetPosition(Vector2(mGame->GetPlayer()->GetPosition().x, GetPosition().y + mHeight * 0.3f));
                sparkEffect->SetEffect(TargetEffect::SwordHit);
            }
            return;
        }
        if (knockBackDirection.x == 1) {
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
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
            if (IsOnScreen()) {
                mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
            }
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

    if (IsOnScreen()) {
        mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
    }
}


void Golem::ManageAnimations() {
    if (mGolemState == State::Punch) {
        if (mIsInvulnerable) {
            mDrawAnimatedComponent->SetAnimation("punchInvulnerable");
        }
        else {
            mDrawAnimatedComponent->SetAnimation("punch");
        }
        mDrawAnimatedComponent->SetAnimFPS(10.0f / mPunchDuration);
    }
    else if (mGolemState == State::RunForward ||
             mGolemState == State::RunAway) {
        if (mIsInvulnerable) {
            mDrawAnimatedComponent->SetAnimation("walkInvulnerable");
        }
        else {
            mDrawAnimatedComponent->SetAnimation("walk");
        }
        mDrawAnimatedComponent->SetAnimFPS(mMoveSpeed / 40);
    }
    else if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else {
        if (mIsInvulnerable) {
            mDrawAnimatedComponent->SetAnimation("idleInvulnerable");
        }
        else {
            mDrawAnimatedComponent->SetAnimation("idle");
        }
        mDrawAnimatedComponent->SetAnimFPS(10.0f);
    }
}

void Golem::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToPunch = mDistToPunch / oldScale * newScale;
    mIdleWidth = mIdleWidth / oldScale * newScale;
    mPunchSpriteWidth = mPunchSpriteWidth / oldScale * newScale;
    mPunchOffsetHitBox = mPunchOffsetHitBox / oldScale * newScale;
    mArenaMinPos.x = mArenaMinPos.x / oldScale * newScale;
    mArenaMinPos.y = mArenaMinPos.y / oldScale * newScale;
    mArenaMaxPos.x = mArenaMaxPos.x / oldScale * newScale;
    mArenaMaxPos.y = mArenaMaxPos.y / oldScale * newScale;

    mFireballWidth = mFireballWidth / oldScale * newScale;
    mFireBallHeight = mFireBallHeight / oldScale * newScale;
    mFireballSpeed = mFireballSpeed / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mWidth == mPunchSpriteWidth) {
        mDrawAnimatedComponent->SetWidth(mIdleWidth * 1.8f * 1.875f);
        mDrawAnimatedComponent->SetHeight(mIdleWidth * 1.8f);
    }
    else {
        mDrawAnimatedComponent->SetWidth(mIdleWidth * 1.8f * 1.875f);
        mDrawAnimatedComponent->SetHeight(mIdleWidth * 1.8f);
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
