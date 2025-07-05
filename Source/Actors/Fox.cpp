//
// Created by roger on 15/05/2025.
//

#include "Fox.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Skill.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DashComponent.h"
#include "../Actors/Sword.h"
#include "../Actors/FireBall.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/DynamicGround.h"


Fox::Fox(Game* game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 10)
{
    mMoneyDrop = 150;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDistToSpotPlayer = 400 * mGame->GetScale();

    mWalkingAroundDuration = 2.0f;
    mWalkingAroundTimer = mWalkingAroundDuration;
    mWalkingAroundMoveSpeed = 50.0f * mGame->GetScale();

    mIsRunning = true;

    mState = State::Stop;

    mStopDuration = 1.5f;
    mStopTimer = 0.0f;

    mDashDuration = 1.0f;
    mDashTimer = 0.0f;
    mMaxDashes = 3;
    mDashCount = 0;

    mRunAwayDuration = 0.8f;
    mRunAwayTimer = 0.0f;

    mFireballDuration = 1.5f;
    mFireballTimer = 0.0f;
    mAlreadyFireballed = false;
    mFireballWidth = 100 * mGame->GetScale();
    mFireBallHeight = 100 * mGame->GetScale();
    mFireballSpeed = 1400 * mGame->GetScale();

    mIsOnGround = true;
    mMaxJumps = 3;
    mJumpCount = 0;
    mJumpForce = -1300.0f * mGame->GetScale();
    mGravity = 3000 * mGame->GetScale();

    mSwordHitPlayer = false;
    mDistToSword = 200 * mGame->GetScale();

    std::string foxAssets = "../Assets/Sprites/Raposa/";
    // mDrawSpriteComponent = new DrawSpriteComponent(this, foxAssets + "Idle.png", 100, 100);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.3f, 0.91f * mWidth * 2.3f,
                                                       foxAssets + "Raposa.png",
                                                       foxAssets + "Raposa.json", 1000);

    std::vector idle = {2};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector run = {3, 4, 5, 6, 7};
    mDrawAnimatedComponent->AddAnimation("run", run);

    std::vector hit = {1};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    std::vector dash = {0};
    mDrawAnimatedComponent->AddAnimation("dash", dash);


    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(16.0f);

    mDashComponent = new DashComponent(this, 1500 * mGame->GetScale(), mDashDuration, 0.5f);
    mSword = new Sword(game, this, mWidth * 3.6f, mHeight * 1.3f, 0.2f, 10.0f);
}

void Fox::OnUpdate(float deltaTime) {
    mIsRunning = false;
    mIsOnGround = false;
    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    if (mFlashTimer < mFlashDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolvePlayerCollision();
    ResolveGroundCollision();
    ResolveEnemyCollision();

    // Gravidade
    if (!mIsOnGround) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                 mRigidBodyComponent->GetVelocity().y
                                                 + mGravity * deltaTime));
    }

    if (mPlayerSpotted) {
        if (!mGame->GetBossMusicHandle().IsValid()) {
            mGame->StartBossMusic(mGame->GetAudio()->PlaySound("Hornet.wav", true));
        }
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
        TriggerBossDefeat();
    }

    ManageAnimations();
    if (mHealthPoints <= mMaxHealthPoints / 2) {
        mStopDuration = 0.5;
        mFireballDuration = 1.0;
        mMaxJumps = 2;
    }
}

void Fox::TriggerBossDefeat() {
    SetState(ActorState::Destroy);
    // Abre chão que estava travando
    for (int id : mUnlockGroundsIds) {
        Ground* g = mGame->GetGroundById(id);
        DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
        if (dynamicGround) {
            dynamicGround->SetIsDecreasing(true);
        }
    }

    // Player ganha pulo duplo
    auto* skill = new Skill(mGame, Skill::SkillType::DoubleJump);
    skill->SetPosition(GetPosition());
    // mGame->GetPlayer()->SetCanFireBall(true);

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


void Fox::ResolveGroundCollision() {
    std::array<bool, 4> collisionSide{};
    std::vector<Ground* > grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                }
                else {
                    collisionSide = {false, false, false, false};
                }

                // colidiu top
                if (collisionSide[0]) {
                    mIsOnGround = true;
                }
                if (collisionSide[2] || collisionSide[3]) {
                    if (mState == State::RunAway) {
                        mRunAwayTimer = mRunAwayDuration;
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                    // Colidiu top
                    if (collisionSide[0]) {
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionSide[1]) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionSide[2]) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionSide[3]) {
                        ReceiveHit(10, Vector2::UnitX);
                    }

                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

void Fox::ResolvePlayerCollision() {
    Player* player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) { // Colisão da Fox com o player
        if (mState == State::Dash) {
            mDashCount = 0;
            mDashTimer = 0;
            mDashComponent->StopDash();
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mState = State::RunAway;
        }
    }
    else if (mSword->GetComponent<AABBComponent>()->Intersect(*player->GetComponent<AABBComponent>())) { // Colisão da sword da fox com o player
        if (!mSwordHitPlayer) {
            player->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
            mSwordHitPlayer = true;
        }
    }
}

void Fox::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mState) {
        case State::Stop:
            Stop(deltaTime);
            break;

        case State::Dash:
            Dash(deltaTime);
            break;

        case State::RunAway:
            RunAway(deltaTime);
            break;

        case State::RunAndSword:
            RunAndSword(deltaTime);
            break;

        case State::Fireball:
            Fireball(deltaTime);
            break;

        case State::Jump:
            Jump(deltaTime);
            break;
    }
}

void Fox::MovementBeforePlayerSpotted() {
    // mIsRunning = true;
    // Player* player = GetGame()->GetPlayer();
    // if (mWalkingAroundTimer > mWalkingAroundDuration) {
    //     SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
    //     mWalkingAroundTimer = 0;
    // }
    // if (mKnockBackTimer >= mKnockBackDuration) {
    //     mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMoveSpeed,
    //                                              mRigidBodyComponent->GetVelocity().y));
    // }
    //
    // // Testa se spotted player
    // Vector2 dist = GetPosition() - player->GetPosition();
    // if (dist.Length() < mDistToSpotPlayer) {
    //     mPlayerSpotted = true;
    // }
}

void Fox::ManageAnimations() {
    if (mIsRunning) {
        mDrawAnimatedComponent->SetAnimation("run");
    }
    else {
        mDrawAnimatedComponent->SetAnimation("idle");
    }
    if (mDashComponent->GetIsDashing()) {
        mDrawAnimatedComponent->SetAnimation("dash");
    }
    if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
    }
}

void Fox::Stop(float deltaTime) {
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    mStopTimer += deltaTime;
    if (mStopTimer < mStopDuration) {
        return;
    }

    mStopTimer = 0;

    if (Random::GetFloat() < 0.5) {
        mState = State::RunAndSword;
    }
    else {
        mState = State::Fireball;
    }
}

void Fox::Dash(float deltaTime) {
    if (mDashCount >= mMaxDashes) {
        mDashCount = 0;
        mState = State::Stop;
        return;
    }
    mDashTimer += deltaTime;
    mDashComponent->UseDash(true);
    if (mDashTimer < mDashDuration) {
        return;
    }

    mDashCount++;
    mDashTimer = 0;
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }
}

void Fox::RunAway(float deltaTime) {
    mIsRunning = true;
    mRunAwayTimer += deltaTime;

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mRigidBodyComponent->GetVelocity().y));

    if (mRunAwayTimer < mRunAwayDuration) {
        return;
    }

    mRunAwayTimer = 0;
    mState = State::Stop;
}

void Fox::RunAndSword(float deltaTime)
{
    mIsRunning = true;
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 3, mRigidBodyComponent->GetVelocity().y));
    if (Math::Abs(dist) < mDistToSword) {
        mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(GetRotation());
        mSword->SetPosition(GetPosition());
        mSwordHitPlayer = false;
        mState = State::RunAway;
    }
}

void Fox::Fireball(float deltaTime)
{
    mFireballTimer += deltaTime;
    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (!mAlreadyFireballed) {
        if (dist < 0) {
            SetRotation(0.0);
        }
        else {
            SetRotation(Math::Pi);
        }

        std::vector<FireBall* > fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetWidth(mFireballWidth);
                f->SetHeight(mFireBallHeight);
                f->SetSpeed(mFireballSpeed);
                f->SetIsFromEnemy();
                f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                break;
            }
        }

        mAlreadyFireballed = true;
    }

    if (mFireballTimer < mFireballDuration) {
        return;
    }

    mAlreadyFireballed = false;
    mFireballTimer = 0;

    if (Random::GetFloat() < 0.5) {
        mState = State::Jump;
    }
    else {
        mState = State::Dash;
    }
}


void Fox::Jump(float deltaTime) {
    if (mJumpCount >= mMaxJumps) {
        if (mIsOnGround) {
            mJumpCount = 0;
            mState = State::Stop;
        }
        return;
    }
    if (mIsOnGround) {
        Player* player = GetGame()->GetPlayer();
        float dist = GetPosition().x - player->GetPosition().x;
        if (dist < 0) {
            SetRotation(0.0);
        }
        else {
            SetRotation(Math::Pi);
        }

        mJumpCount++;
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 2, mJumpForce));
    }
}

void Fox::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mFireballWidth = mFireballWidth / oldScale * newScale;
    mFireBallHeight = mFireBallHeight / oldScale * newScale;
    mFireballSpeed = mFireballSpeed / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;
    mDistToSword = mDistToSword / oldScale * newScale;
    mDashComponent->SetDashSpeed(mDashComponent->GetDashSpeed() / oldScale * newScale);

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth * 2.3f);
    mDrawAnimatedComponent->SetHeight(0.91f * mWidth * 2.3f);

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
