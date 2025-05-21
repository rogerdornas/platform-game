//
// Created by roger on 15/05/2025.
//

#include "Fox.h"
#include "Actor.h"

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


Fox::Fox(Game *game, float width, float height, float moveSpeed, float healthPoints)
    : Enemy(game, width, height, moveSpeed, healthPoints, 10)
{
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mPlayerSpotted = false;
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

    mIsOnGround = true;
    mMaxJumps = 3;
    mJumpCount = 0;
    mJumpForce = -1300.0f * mGame->GetScale();

    mSwordHitPlayer = false;

    std::string foxAssets = "../Assets/Sprites/Raposa 2/";
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

void Fox::OnUpdate(float deltaTime)
{
    mIsRunning = false;
    mIsOnGround = false;
    mKnockBackTimer += deltaTime;
    mWalkingAroundTimer += deltaTime;

    if (mFlashTimer < mFlashDuration)
        mFlashTimer += deltaTime;

    else
        mIsFlashing = false;


    // Gravidade
    if (!mIsOnGround)
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                 mRigidBodyComponent->GetVelocity().y
                                                 + 3000 * mGame->GetScale() * deltaTime));

    ResolvePlayerCollision();
    ResolveGroundCollision();

    if (mPlayerSpotted)
        MovementAfterPlayerSpotted(deltaTime);

    else
        MovementBeforePlayerSpotted();

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000 * mGame->GetScale())
        SetPosition(Vector2::Zero);

    // Se morreu
    if (Died())
        SetState(ActorState::Destroy);

    ManageAnimations();

    // SDL_Log("Life: %f", mHealthPoints);
    if (mHealthPoints <= 100)
    {
        mStopDuration = 0.5;
        mFireballDuration = 1.0;
        mMaxJumps = 2;
    }
}

void Fox::ResolveGroundCollision()
{
    std::array<bool, 4> collisionSide{};
    std::vector<Ground *> grounds = GetGame()->GetGrounds();
    if (!grounds.empty())
    {
        for (Ground *g: grounds)
        {
            if (!g->GetIsSpike())
            { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>()))
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());

                else
                    collisionSide = {false, false, false, false};

                // colidiu top
                if (collisionSide[0])
                    mIsOnGround = true;

                if (collisionSide[2] || collisionSide[3])
                    if (mState == State::RunAway)
                        mRunAwayTimer = mRunAwayDuration;
            }
            else if (g->GetIsSpike())
            { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>()))
                    SetPosition(Vector2::Zero);
            }
        }
    }
}

void Fox::ResolvePlayerCollision()
{
    Player *player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>()))
    { // Colisão da Fox com o ‘player’
        if (mState == State::Dash)
        {
            mDashCount = 0;
            mDashTimer = 0;
            mDashComponent->StopDash();
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
            mState = State::RunAway;
        }
    }
    else if (mSword->GetComponent<AABBComponent>()->Intersect(*player->GetComponent<AABBComponent>()))
    { // Colisão da sword da ‘fox’ com o ‘player’
        if (!mSwordHitPlayer)
        {
            player->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
            mSwordHitPlayer = true;
        }
    }
}

void Fox::MovementAfterPlayerSpotted(float deltaTime)
{
    switch (mState)
    {
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

void Fox::MovementBeforePlayerSpotted()
{
    mIsRunning = true;
    Player *player = GetGame()->GetPlayer();
    if (mWalkingAroundTimer > mWalkingAroundDuration)
    {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mWalkingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration)
        mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMoveSpeed,
                                                 mRigidBodyComponent->GetVelocity().y));

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer)
        mPlayerSpotted = true;
}

void Fox::ManageAnimations()
{
    if (mIsRunning)
        mDrawAnimatedComponent->SetAnimation("run");

    else
        mDrawAnimatedComponent->SetAnimation("idle");

    if (mDashComponent->GetIsDashing())
        mDrawAnimatedComponent->SetAnimation("dash");

    if (mIsFlashing)
        mDrawAnimatedComponent->SetAnimation("hit");
}

void Fox::Stop(float deltaTime)
{
    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0)
        SetRotation(0.0);

    else
        SetRotation(Math::Pi);

    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    mStopTimer += deltaTime;
    if (mStopTimer < mStopDuration)
        return;

    mStopTimer = 0;

    if (Random::GetFloat() < 0.5)
        mState = State::RunAndSword;

    else
        mState = State::Fireball;

    // if (Math::Abs(dist) < 700) {
    //     mState = State::RunAndSword;
    // }
    // else {
    //     mState = State::Fireball;
    // }
}

void Fox::Dash(float deltaTime)
{
    // Player* player = GetGame()->GetPlayer();
    // float dist = GetPosition().x - player->GetPosition().x;
    // if (dist < 0) {
    //     SetRotation(0.0);
    // }
    // else {
    //     SetRotation(Math::Pi);
    // }

    if (mDashCount >= mMaxDashes)
    {
        mDashCount = 0;
        mState = State::Stop;
        return;
    }
    mDashTimer += deltaTime;
    mDashComponent->UseDash(true);
    if (mDashTimer < mDashDuration)
        return;

    mDashCount++;
    mDashTimer = 0;
    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0)
        SetRotation(0.0);

    else
        SetRotation(Math::Pi);

    // mState = State::Stop;
}

void Fox::RunAway(float deltaTime)
{
    mIsRunning = true;
    mRunAwayTimer += deltaTime;

    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;

    if (dist > 0)
        SetRotation(0.0);

    else
        SetRotation(Math::Pi);

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 4, mRigidBodyComponent->GetVelocity().y));

    if (mRunAwayTimer < mRunAwayDuration)
        return;

    mRunAwayTimer = 0;
    mState = State::Stop;
}

void Fox::RunAndSword(float deltaTime)
{
    mIsRunning = true;
    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0)
        SetRotation(0.0);

    else
        SetRotation(Math::Pi);

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mMoveSpeed * 3, mRigidBodyComponent->GetVelocity().y));
    if (Math::Abs(dist) < 200 * mGame->GetScale())
    {
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
    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (!mAlreadyFireballed)
    {
        if (dist < 0)
            SetRotation(0.0);

        else
            SetRotation(Math::Pi);

        std::vector<FireBall *> fireBalls = GetGame()->GetFireBalls();
        for (FireBall *f: fireBalls)
            if (f->GetState() == ActorState::Paused)
            {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetWidth(160 * mGame->GetScale());
                f->SetHeight(80 * mGame->GetScale());
                f->SetSpeed(1400 * mGame->GetScale());
                f->SetIsFromEnemy();
                f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                break;
            }

        mAlreadyFireballed = true;
    }

    if (mFireballTimer < mFireballDuration)
        return;

    mAlreadyFireballed = false;
    mFireballTimer = 0;

    if (Random::GetFloat() < 0.5)
        mState = State::Jump;

    else
        mState = State::Dash;

    // if (Math::Abs(dist) < 500) {
    //     mState = State::Jump;
    // }
    // else {
    //     mState = State::Dash;
    // }
}


void Fox::Jump(float deltaTime)
{
    if (mJumpCount >= mMaxJumps)
    {
        if (mIsOnGround)
        {
            mJumpCount = 0;
            mState = State::Stop;
        }
        return;
    }
    if (mIsOnGround)
    {
        Player *player = GetGame()->GetPlayer();
        float dist = GetPosition().x - player->GetPosition().x;
        if (dist < 0)
            SetRotation(0.0);

        else
            SetRotation(Math::Pi);

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
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mDashComponent->SetDashSpeed(mDashComponent->GetDashSpeed() / oldScale * newScale);

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

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);

}
