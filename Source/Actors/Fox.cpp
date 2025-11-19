//
// Created by roger on 15/05/2025.
//

#include "Fox.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Skill.h"
#include "../Game.h"
#include "../HUD.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/DashComponent.h"
#include "../Actors/Sword.h"
#include "../Actors/FireBall.h"
#include "../Random.h"
#include "../Actors/DynamicGround.h"


Fox::Fox(Game* game)
    :Enemy(game)
    ,mDistToSpotPlayer(400 * mGame->GetScale())
    ,mWalkingAroundDuration(2.0f)
    ,mWalkingAroundTimer(0.0f)
    ,mWalkingAroundMoveSpeed(50.0f * mGame->GetScale())

    ,mFoxState(State::Stop)

    ,mIsRunning(true)

    ,mStopDuration(1.5f)
    ,mStopTimer(0.0f)

    ,mDashDuration(0.9f)
    ,mDashTimer(0.0f)
    ,mMaxDashes(3)
    ,mDashCount(0)

    ,mRunAwayDuration(0.8f)
    ,mRunAwayTimer(0.0f)

    ,mFireballDuration(1.5f)
    ,mFireballTimer(0.0f)
    ,mAlreadyFireballed(false)
    ,mFireballWidth(100 * mGame->GetScale())
    ,mFireBallHeight(100 * mGame->GetScale())
    ,mFireballSpeed(1400 * mGame->GetScale())

    ,mIsOnGround(true)
    ,mMaxJumps(3)
    ,mJumpCount(0)
    ,mJumpForce(-1300.0f * mGame->GetScale())
    ,mGravity(3000 * mGame->GetScale())

    ,mSword(nullptr)
    ,mSwordHitPlayer(false)
    ,mDistToSword(200 * mGame->GetScale())
    ,mRunAndSwordProbability(0.5f)

    ,mDashComponent(nullptr)
{
    mWidth = 100 * mGame->GetScale();
    mHeight = 170 * mGame->GetScale();
    mMoveSpeed = 300 * mGame->GetScale();
    mHealthPoints = 700;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 150;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    std::string foxAssets = "../Assets/Sprites/Raposa/";

    mDrawComponent = new AnimatorComponent(this, foxAssets + "Raposa.png",
                                                       foxAssets + "Raposa.json",
                                                       mWidth * 2.3f, 0.91f * mWidth * 2.3f, 998);

    std::vector idle = {2};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector run = {3, 4, 5, 6, 7};
    mDrawComponent->AddAnimation("run", run);

    std::vector hit = {1};
    mDrawComponent->AddAnimation("hit", hit);

    std::vector dash = {0};
    mDrawComponent->AddAnimation("dash", dash);


    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(16.0f);

    mDashComponent = new DashComponent(this, 1500 * mGame->GetScale(), mDashDuration * 0.95f, 0.5f);
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
        mGame->GetHUD()->StartBossFight(this);
        if (!mGame->GetBossMusicHandle().IsValid()) {
            mGame->StartBossMusic(mGame->GetAudio()->PlaySound("Hornet.wav", true));
        }
        MovementAfterPlayerSpotted(deltaTime);
    }
    else {
        MovementBeforePlayerSpotted();
    }

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    if (mDrawComponent) {
        ManageAnimations();
    }

    if (mHealthPoints <= mMaxHealthPoints / 2) {
        mStopDuration = 0.5;
        mFireballDuration = 1.0;
        mMaxJumps = 2;
    }
}

void Fox::TriggerBossDefeat() {
    // Player ganha pulo duplo
    if (mGame->GetPlayer()->GetMaxJumpsInAir() == 0) {
        auto* skill = new Skill(mGame, Skill::SkillType::DoubleJump);
        skill->SetPosition(GetPosition());
    }

    mGame->SetWorldFlag("FoxDefeated", true);
    mGame->StopBossMusic();
}


void Fox::ResolveGroundCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground* > grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                }
                else {
                    collisionNormal = Vector2::Zero;
                }

                // colidiu top
                if (collisionNormal == Vector2::NegUnitY) {
                    mIsOnGround = true;
                }
                if (collisionNormal == Vector2::NegUnitX || collisionNormal == Vector2::UnitX) {
                    if (mFoxState == State::RunAway) {
                        mRunAwayTimer = mRunAwayDuration;
                    }
                    if (mFoxState == State::Dash) {
                        if (collisionNormal == Vector2::NegUnitX) {
                            mDashTimer = mDashDuration;
                            SetPosition(GetPosition() - Vector2(32, 0) * mGame->GetScale());
                        }
                        else {
                            mDashTimer = mDashDuration;
                            SetPosition(GetPosition() + Vector2(32, 0) * mGame->GetScale());
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (collisionNormal == Vector2::NegUnitY) {
                        ReceiveHit(10, Vector2::NegUnitY);
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

void Fox::ResolvePlayerCollision() {
    Player* player = GetGame()->GetPlayer();
    if (mColliderComponent->Intersect(*player->GetComponent<ColliderComponent>())) { // Colisão da Fox com o player
        if (mFoxState == State::Dash) {
            // mDashCount = 0;
            // mDashTimer = 0;
            // mDashComponent->StopDash();
            // mFoxState = State::RunAway;
        }
    }
    else if (mSword->GetComponent<ColliderComponent>()->Intersect(*player->GetComponent<ColliderComponent>())) { // Colisão da sword da fox com o player
        if (!mSwordHitPlayer) {
            player->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
            mSwordHitPlayer = true;
        }
    }
}

void Fox::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mFoxState) {
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
        mDrawComponent->SetAnimation("run");
    }
    else {
        mDrawComponent->SetAnimation("idle");
    }
    if (mDashComponent->GetIsDashing()) {
        mDrawComponent->SetAnimation("dash");
    }
    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
    }
}

void Fox::Stop(float deltaTime) {
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

    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    mStopTimer += deltaTime;
    if (mStopTimer < mStopDuration) {
        return;
    }

    mStopTimer = 0;

    if (Random::GetFloat() < 0.5) {
        mFoxState = State::RunAndSword;
    }
    else {
        mFoxState = State::Fireball;
    }

    // Controla probabilidade de combo de espada para não ficar spamando
    if (mFoxState == State::RunAndSword) {
        mRunAndSwordProbability -= 0.1;
    }
    else {
        mRunAndSwordProbability = 0.5;
    }
}

void Fox::Dash(float deltaTime) {
    if (mDashCount >= mMaxDashes) {
        mDashCount = 0;
        mDashTimer = 0;
        mFoxState = State::Stop;
        return;
    }

    if (mDashTimer == 0) {
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
        mDashComponent->UseDash(true);
    }

    mDashTimer += deltaTime;

    if (mDashTimer >= mDashDuration) {
        mDashCount++;
        mDashComponent->StopDash();
        mDashTimer = 0;
    }
}

void Fox::RunAway(float deltaTime) {
    mIsRunning = true;
    mRunAwayTimer += deltaTime;

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0) {
        SetRotation(0.0);
        SetScale(Vector2(1,1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1,1));
    }
    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mRigidBodyComponent->GetVelocity().y));

    if (mRunAwayTimer < mRunAwayDuration) {
        return;
    }

    mRunAwayTimer = 0;
    mFoxState = State::Stop;
}

void Fox::RunAndSword(float deltaTime)
{
    mIsRunning = true;
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

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 3, mRigidBodyComponent->GetVelocity().y));
    if (Math::Abs(dist) < mDistToSword) {
        mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
        mSword->SetState(ActorState::Active);
        // detecta se player está em cima do boss
        bool up = (player->GetPosition().y < GetPosition().y) &&
                  (player->GetPosition().x > GetPosition().x - mWidth &&
                   player->GetPosition().x < GetPosition().x + mWidth);

        if (up) {
            mSword->SetRotation(3 * Math::Pi / 2);
            mSword->SetTransformRotation(3 * Math::Pi / 2);
            mSword->SetScale(Vector2(1, 1));
        }
        else {
            mSword->SetRotation(GetRotation());
            mSword->SetTransformRotation(0.0f);
            mSword->SetScale(Vector2(GetForward().x, 1));
        }
        mSword->SetPosition(GetPosition());
        mSwordHitPlayer = false;
        mFoxState = State::RunAway;
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
            SetScale(Vector2(1, 1));
        }
        else {
            SetRotation(Math::Pi);
            SetScale(Vector2(-1, 1));
        }

        std::vector<FireBall* > fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetScale(Vector2(GetForward().x, 1));
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

    float jumpProbability = 0.5f;

    // detecta se player está em cima do boss
    bool up = (player->GetPosition().y < GetPosition().y) &&
              (player->GetPosition().x > GetPosition().x - mWidth &&
               player->GetPosition().x < GetPosition().x + mWidth);

    if (up) {
        jumpProbability = 0.8f;
    }

    if (Random::GetFloat() < jumpProbability) {
        mFoxState = State::Jump;
    }
    else {
        mFoxState = State::Dash;
    }
}


void Fox::Jump(float deltaTime) {
    if (mJumpCount >= mMaxJumps) {
        if (mIsOnGround) {
            mJumpCount = 0;
            mFoxState = State::Stop;
        }
        return;
    }
    if (mIsOnGround) {
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 2.3f);
    //     mDrawAnimatedComponent->SetHeight(0.91f * mWidth * 2.3f);
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
