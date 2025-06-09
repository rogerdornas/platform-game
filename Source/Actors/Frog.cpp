//
// Created by roger on 02/06/2025.
//

#include "Frog.h"
#include "Actor.h"
#include "Effect.h"
#include "FrogTongue.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/DynamicGround.h"


Frog::Frog(Game *game, float width, float height, float moveSpeed, float healthPoints)
    : Enemy(game, width, height, moveSpeed, healthPoints, 10)
{
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDistToSpotPlayer = 500 * mGame->GetScale();

    mWalkingAroundDuration = 2.0f;
    mWalkingAroundTimer = mWalkingAroundDuration;
    mWalkingAroundMoveSpeed = 50.0f * mGame->GetScale();

    mIsRunning = true;

    mState = FrogState::frogStop;

    mStopDuration = 1.5f;
    mStopTimer = 0.5f;

    mIsOnGround = true;
    mMaxJumps = 6;
    mJumpCount = 0;
    mJumpForce = 1500.0f * mGame->GetScale();
    mDurationBetweenJumps = 0.3f;
    mTimerBetweenJumps = 0.0f;

    mWallPosition = FrogWallSide::frogBottom;

    mMinDistFromEdge = 256 * mGame->GetScale();
    mAttackJumpInterval = 2;
    mGravity = 3000 * mGame->GetScale();
    mTongueDuration = 1.2f;
    mTongueTimer = 0.0f;
    mIsLicking = false;
    mJumpComboProbability = 0.5f;

    std::string frogAssets = "../Assets/Sprites/Frog/";

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.3f, mHeight * 1.3f,
                                                       frogAssets + "Frog.png",
                                                       frogAssets + "Frog.json", 999);

    std::vector hitIdle = {0};
    mDrawAnimatedComponent->AddAnimation("hitIdle", hitIdle);

    std::vector hitJump = {1};
    mDrawAnimatedComponent->AddAnimation("hitJump", hitJump);

    std::vector hitSide = {2};
    mDrawAnimatedComponent->AddAnimation("hitSide", hitSide);

    std::vector hitTongue = {3};
    mDrawAnimatedComponent->AddAnimation("hitTongue", hitTongue);

    std::vector idle = {4};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector jump = {5};
    mDrawAnimatedComponent->AddAnimation("jump", jump);

    std::vector side = {6};
    mDrawAnimatedComponent->AddAnimation("side", side);

    std::vector tongue = {7};
    mDrawAnimatedComponent->AddAnimation("tongue", tongue);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(16.0f);

    mTongue = new FrogTongue(mGame, this, 20.0f);
    mTongue->SetDuration(mTongueDuration);
}

void Frog::OnUpdate(float deltaTime)
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
    // if (!mIsOnGround)
    //     mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
    //                                              mRigidBodyComponent->GetVelocity().y
    //                                              + mGravity * deltaTime));

    ResolvePlayerCollision();
    ResolveGroundCollision();

    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;
    std::vector<Vector2> vertices;
    if (mIsOnGround) {
        if (mWallPosition == FrogWallSide::frogBottom || mWallPosition == FrogWallSide::frogTop) {
            v1 = Vector2(-mWidth / 2, -mHeight / 2);
            v2 = Vector2(mWidth / 2, -mHeight / 2);
            v3 = Vector2(mWidth / 2, mHeight / 2);
            v4 = Vector2(-mWidth / 2, mHeight / 2);
            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);
        }
        else if (mWallPosition == FrogWallSide::frogLeft || mWallPosition == FrogWallSide::frogRight) {
            v1 = Vector2(-mHeight / 2, -mWidth / 2);
            v2 = Vector2(mHeight / 2, -mWidth / 2);
            v3 = Vector2(mHeight / 2, mWidth / 2);
            v4 = Vector2(-mHeight / 2, mWidth / 2);
            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);
        }
    }
    else {
        v1 = Vector2(-mHeight / 2, -mHeight / 2);
        v2 = Vector2(mHeight / 2, -mHeight / 2);
        v3 = Vector2(mHeight / 2, mHeight / 2);
        v4 = Vector2(-mHeight / 2, mHeight / 2);
        vertices.emplace_back(v1);
        vertices.emplace_back(v2);
        vertices.emplace_back(v3);
        vertices.emplace_back(v4);
    }

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);

    if (mDrawSpriteComponent)
    {
        mDrawSpriteComponent->SetWidth(mWidth * 1.3f);
        mDrawSpriteComponent->SetHeight(mHeight * 1.3f);
    }
    if (mDrawAnimatedComponent)
    {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);
    }

    if (mPlayerSpotted)
        MovementAfterPlayerSpotted(deltaTime);

    else
        MovementBeforePlayerSpotted();

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000 * mGame->GetScale())
        SetPosition(Vector2::Zero);

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    ManageAnimations();

    // SDL_Log("Life: %f", mHealthPoints);
    if (mHealthPoints <= 100)
    {
        mStopDuration = 1.0;
        mMaxJumps = 12;
        mJumpForce = 1900 * mGame->GetScale();
        mDurationBetweenJumps = 0.15;
        mAttackJumpInterval = 3;
        mTongueDuration = 0.8f;
        mTongue->SetDuration(mTongueDuration);
    }
}

void Frog::TriggerBossDefeat() {
    SetState(ActorState::Destroy);
    // Abre chão que estava travando
    for (int id : mUnlockGroundsIds) {
        Ground *g = mGame->GetGroundById(id);
        DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
        if (dynamicGround) {
            dynamicGround->SetIsDecreasing(true);
        }
    }

    // // Ground* g1 = mGame->GetGroundById(140);
    // // Ground* g2 = mGame->GetGroundById(141);
    // Ground* g1 = mGame->GetGroundById(18);
    // Ground* g2 = mGame->GetGroundById(19);
    // DynamicGround* dynamicGround1 = dynamic_cast<DynamicGround*>(g1);
    // dynamicGround1->SetIsDecreasing(true);
    // DynamicGround* dynamicGround2 = dynamic_cast<DynamicGround*>(g2);
    // dynamicGround2->SetIsDecreasing(true);

    mTongue->SetState(ActorState::Destroy);

    mGame->GetPlayer()->SetCanWallSlide(true);

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
    circleBlur->SetEffect(TargetEffect::circle);
    circleBlur->EnemyDestroyed();

    mGame->StopBossMusic();
}


void Frog::ResolveGroundCollision()
{
    std::array<bool, 4> collisionSide{};
    std::vector<Ground *> grounds = GetGame()->GetGrounds();
    if (!grounds.empty())
    {
        for (Ground *g: grounds)
        {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                    mIsOnGround = true;
                    if (collisionSide[0]) {
                        mWallPosition = FrogWallSide::frogBottom;
                    }
                    if (collisionSide[1]) {
                        mWallPosition = FrogWallSide::frogTop;
                    }
                    if (collisionSide[2]) {
                        mWallPosition = FrogWallSide::frogRight;
                    }
                    if (collisionSide[3]) {
                        mWallPosition = FrogWallSide::frogLeft;
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>()))
                    SetPosition(Vector2::Zero);
            }
        }
    }
}

void Frog::ResolvePlayerCollision()
{
    Player *player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
    }
}

void Frog::MovementAfterPlayerSpotted(float deltaTime)
{
    switch (mState)
    {
        case FrogState::frogStop:
            Stop(deltaTime);
            break;

        case FrogState::frogJumpCombo:
            JumpCombo(deltaTime);
            break;

        case FrogState::frogTongue:
            Tongue(deltaTime);
        break;
    }
}

void Frog::MovementBeforePlayerSpotted()
{
    Player *player = GetGame()->GetPlayer();
    // mIsRunning = true;
    // if (mWalkingAroundTimer > mWalkingAroundDuration)
    // {
    //     SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
    //     mWalkingAroundTimer = 0;
    // }
    // if (mKnockBackTimer >= mKnockBackDuration)
    //     mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mWalkingAroundMoveSpeed,
    //                                              mRigidBodyComponent->GetVelocity().y));

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
        mGame->StarBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav"));
    }
}

void Frog::ManageAnimations() {
    if (mState == FrogState::frogTongue) {
        mDrawAnimatedComponent->SetAnimation("tongue");
    }
    else if (mIsOnGround) {
        mDrawAnimatedComponent->SetAnimation("idle");
        if (mWallPosition == FrogWallSide::frogBottom) {
            mDrawAnimatedComponent->SetAnimation("side");

            if (mDrawAnimatedComponent)
            {
                mDrawAnimatedComponent->UseRotation(false);
                mDrawAnimatedComponent->SetOffsetRotation(0.0f);
                mDrawAnimatedComponent->UseFlip(false);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
            }
        }
        else if (mWallPosition == FrogWallSide::frogTop) {
            if (mDrawAnimatedComponent)
            {
                mDrawAnimatedComponent->UseRotation(true);
                mDrawAnimatedComponent->SetOffsetRotation(0.0f);
                mDrawAnimatedComponent->UseFlip(false);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
            }
        }
        else if (mWallPosition == FrogWallSide::frogLeft) {
            if (mDrawAnimatedComponent)
            {
                mDrawAnimatedComponent->UseRotation(true);
                mDrawAnimatedComponent->SetOffsetRotation(0.0f);
                mDrawAnimatedComponent->UseFlip(false);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
            }
        }
        else if (mWallPosition == FrogWallSide::frogRight) {
            if (mDrawAnimatedComponent)
            {
                mDrawAnimatedComponent->UseRotation(true);
                mDrawAnimatedComponent->SetOffsetRotation(0.0f);
                mDrawAnimatedComponent->UseFlip(false);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
            }
        }
    }
    else {
        if (mDrawAnimatedComponent)
        {
            mDrawAnimatedComponent->SetAnimation("jump");
            mDrawAnimatedComponent->UseRotation(true);
            mDrawAnimatedComponent->UseFlip(false);
            Vector2 vel = mRigidBodyComponent->GetVelocity();
            float offset = 67.5f;
            if (vel.x > 0) {
                mDrawAnimatedComponent->SetOffsetRotation(offset);
            }
            else if (vel.y > 0) {
                offset += Math::ToDegrees(Math::Pi - GetRotation());
                mDrawAnimatedComponent->SetOffsetRotation(offset);
                mDrawAnimatedComponent->UseFlip(true);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
            }
            else if (vel.y <= 0) {
                offset = Math::ToDegrees(Math::Pi - GetRotation());
                mDrawAnimatedComponent->SetOffsetRotation(offset);
                mDrawAnimatedComponent->UseFlip(true);
                mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
            }
        }
        // mDrawAnimatedComponent->SetAnimation("jump");
        // // SDL_Log("%f", GetRotation());
        // if (mDestinyWall == FrogWallSide::FrogBottom) {
        //     Vector2 v1(-mHeight / 2, -mHeight / 2);
        //     Vector2 v2(mHeight / 2, -mHeight / 2);
        //     Vector2 v3(mHeight / 2, mHeight / 2);
        //     Vector2 v4(-mHeight / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent)
        //         mDrawPolygonComponent->SetVertices(vertices);
        //
        //     if (mDrawSpriteComponent)
        //     {
        //         mDrawSpriteComponent->SetWidth(mWidth * 1.3f);
        //         mDrawSpriteComponent->SetHeight(mHeight * 1.3f);
        //     }
        //     if (mDrawAnimatedComponent)
        //     {
        //         mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
        //         mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);
        //         mDrawAnimatedComponent->UseRotation(true);
        //         mDrawAnimatedComponent->SetOffsetRotation(45.0f);
        //         mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
        //     }
        // }
        // else if (mDestinyWall == FrogWallSide::FrogTop) {
        //     Vector2 v1(-mHeight / 2, -mHeight / 2);
        //     Vector2 v2(mHeight / 2, -mHeight / 2);
        //     Vector2 v3(mHeight / 2, mHeight / 2);
        //     Vector2 v4(-mHeight / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent)
        //         mDrawPolygonComponent->SetVertices(vertices);
        //
        //     if (mDrawSpriteComponent)
        //     {
        //         mDrawSpriteComponent->SetWidth(mWidth * 1.3f);
        //         mDrawSpriteComponent->SetHeight(mHeight * 1.3f);
        //     }
        //     if (mDrawAnimatedComponent)
        //     {
        //         mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
        //         mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);
        //         mDrawAnimatedComponent->UseRotation(true);
        //         mDrawAnimatedComponent->SetOffsetRotation(45.0f);
        //         if (mRigidBodyComponent->GetVelocity().x < 0) {
        //             SDL_Log("aqui");
        //             mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
        //         }
        //         else {
        //             mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
        //         }
        //     }
        // }
        // else if (mDestinyWall == FrogWallSide::FrogLeft) {
        //     Vector2 v1(-mHeight / 2, -mHeight / 2);
        //     Vector2 v2(mHeight / 2, -mHeight / 2);
        //     Vector2 v3(mHeight / 2, mHeight / 2);
        //     Vector2 v4(-mHeight / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent)
        //         mDrawPolygonComponent->SetVertices(vertices);
        //
        //     if (mDrawSpriteComponent)
        //     {
        //         mDrawSpriteComponent->SetWidth(mWidth * 1.3f);
        //         mDrawSpriteComponent->SetHeight(mHeight * 1.3f);
        //     }
        //     if (mDrawAnimatedComponent)
        //     {
        //         mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
        //         mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);
        //         mDrawAnimatedComponent->UseRotation(true);
        //         mDrawAnimatedComponent->SetOffsetRotation(90.0f);
        //         mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        //     }
        // }
        // else if (mDestinyWall == FrogWallSide::FrogRight) {
        //     Vector2 v1(-mHeight / 2, -mHeight / 2);
        //     Vector2 v2(mHeight / 2, -mHeight / 2);
        //     Vector2 v3(mHeight / 2, mHeight / 2);
        //     Vector2 v4(-mHeight / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent)
        //         mDrawPolygonComponent->SetVertices(vertices);
        //
        //     if (mDrawSpriteComponent)
        //     {
        //         mDrawSpriteComponent->SetWidth(mWidth * 1.3f);
        //         mDrawSpriteComponent->SetHeight(mHeight * 1.3f);
        //     }
        //     if (mDrawAnimatedComponent)
        //     {
        //         mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
        //         mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);
        //         mDrawAnimatedComponent->UseRotation(true);
        //         mDrawAnimatedComponent->SetOffsetRotation(45.0f);
        //         mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
        //     }
        // }
    }
    if (mIsFlashing) {
        if (mIsOnGround) {
            if (mWallPosition == FrogWallSide::frogBottom) {
                mDrawAnimatedComponent->SetAnimation("hitSide");
            }
            else {
                mDrawAnimatedComponent->SetAnimation("hitIdle");
            }
        }
        else {
            mDrawAnimatedComponent->SetAnimation("hitJump");
        }
        if (mState == FrogState::frogTongue) {
            mDrawAnimatedComponent->SetAnimation("hitTongue");
        }
    }


    // if (mIsRunning) {
    //     mDrawAnimatedComponent->SetAnimation("run");
    // }
    // else {
    //     mDrawAnimatedComponent->SetAnimation("idle");
    // }
    // if (mDashComponent->GetIsDashing()) {
    //     mDrawAnimatedComponent->SetAnimation("dash");
    // }
    // if (mIsFlashing) {
    //     mDrawAnimatedComponent->SetAnimation("hit");
    // }
}

void Frog::Stop(float deltaTime) {
    Player *player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mStopTimer += deltaTime;
    if (mStopTimer < mStopDuration)
        return;

    mStopTimer = 0;
    if (Random::GetFloat() < mJumpComboProbability) {
        mState = FrogState::frogJumpCombo;
    }
    else {
        mState = FrogState::frogTongue;
        mIsLicking = true;
    }

    // Controla probabilidade de combo de pulo para não ficar spamando
    if (mState == FrogState::frogJumpCombo) {
        mJumpComboProbability -= 0.1;
    }
    else {
        mJumpComboProbability = 0.5;
    }

    // if (Math::Abs(dist) < 700) {
    //     mState = State::RunAndSword;
    // }
    // else {
    //     mState = State::Fireball;
    // }
}



void Frog::JumpCombo(float deltaTime)
{
    if (mIsOnGround) {
        Player *player = GetGame()->GetPlayer();
        float dist = GetPosition().x - player->GetPosition().x;
        switch (mWallPosition) {
            case FrogWallSide::frogBottom:
                if (dist < 0) {
                    SetRotation(0.0);
                }
                else {
                    SetRotation(Math::Pi);
                }
            break;
            case FrogWallSide::frogTop:
                SetRotation(Math::Pi);
            break;
            case FrogWallSide::frogLeft:
                SetRotation(Math::Pi / 2);
            break;
            case FrogWallSide::frogRight:
                SetRotation(3 * Math::Pi / 2);
            break;
        }
        // SDL_Log("Logo antes de comparar mJumpCount: %d", mJumpCount);
        if (mJumpCount >= mMaxJumps) {
            // SDL_Log("aqui");
            mJumpCount = 0;
            mState = FrogState::frogStop;
            return;
        }
        mTimerBetweenJumps += deltaTime;
        if (mTimerBetweenJumps >= mDurationBetweenJumps) {
            mTimerBetweenJumps = 0;
            mJumpCount++;
            // SDL_Log("logo após incrementar mJumoCount: %d", mJumpCount);

            Vector2 targetPos;
            float dx;
            float dy;
            float angle;
            if (mJumpCount == mMaxJumps) {
                mDestinyWall = FrogWallSide::frogBottom;
                targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                targetPos.y = mArenaMaxPos.y;
                dx = targetPos.x - GetPosition().x;
                dy = targetPos.y - GetPosition().y;

                angle = Math::Atan2(dy, dx);
                // Ajustar para intervalo [0, 2*pi)
                if (angle < 0)
                    angle += 2 * Math::Pi;

                SetRotation(angle);
                // SDL_Log("%f", GetRotation());
                mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
            }
            else if ((mJumpCount % mAttackJumpInterval) == 0) {
                dx = player->GetPosition().x - GetPosition().x;
                dy = player->GetPosition().y - GetPosition().y;

                angle = Math::Atan2(dy, dx);
                // Ajustar para intervalo [0, 2*pi)
                if (angle < 0)
                    angle += 2 * Math::Pi;

                SetRotation(angle);
                // SDL_Log("%f", GetRotation());
                mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
                // SDL_Log("Dentro do múltiplo de 3 mJumoCount: %d", mJumpCount);
            }
            else {
                mDestinyWall  = mWallPosition;
                std::vector<FrogWallSide> validSides = {frogTop, frogLeft, frogRight};
                while (mDestinyWall == mWallPosition) {
                    int index = Random::GetIntRange(0, 2);
                    mDestinyWall = validSides[index];
                    // SDL_Log("Frog is here: %d", mWichWallFrogIs);
                    // SDL_Log("%d", mDestinyWall);
                }
                switch (mDestinyWall) {
                    case FrogWallSide::frogBottom:
                        targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                        targetPos.y = mArenaMaxPos.y;
                        dx = targetPos.x - GetPosition().x;
                        dy = targetPos.y - GetPosition().y;

                        angle = Math::Atan2(dy, dx);
                        // Ajustar para intervalo [0, 2*pi)
                        if (angle < 0)
                            angle += 2 * Math::Pi;

                        SetRotation(angle);
                        // SDL_Log("%f", GetRotation());
                        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
                        break;
                    case FrogWallSide::frogTop:
                        targetPos.x = Random::GetFloatRange(mArenaMinPos.x + mMinDistFromEdge, mArenaMaxPos.x - mMinDistFromEdge);
                        targetPos.y = mArenaMinPos.y;
                        dx = targetPos.x - GetPosition().x;
                        dy = targetPos.y - GetPosition().y;

                        angle = Math::Atan2(dy, dx);
                        // Ajustar para intervalo [0, 2*pi)
                        if (angle < 0)
                            angle += 2 * Math::Pi;

                        SetRotation(angle);
                        // SDL_Log("%f", GetRotation());
                        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
                        break;
                    case FrogWallSide::frogRight:
                        targetPos.x = mArenaMaxPos.x;
                        targetPos.y = Random::GetFloatRange(mArenaMinPos.y + mMinDistFromEdge, mArenaMaxPos.y - mMinDistFromEdge);
                        dx = targetPos.x - GetPosition().x;
                        dy = targetPos.y - GetPosition().y;

                        angle = Math::Atan2(dy, dx);
                        // Ajustar para intervalo [0, 2*pi)
                        if (angle < 0)
                            angle += 2 * Math::Pi;

                        SetRotation(angle);
                        // SDL_Log("%f", GetRotation());
                        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
                        break;
                    case FrogWallSide::frogLeft:
                        targetPos.x = mArenaMinPos.x;
                        targetPos.y = Random::GetFloatRange(mArenaMinPos.y + mMinDistFromEdge, mArenaMaxPos.y - mMinDistFromEdge);
                        dx = targetPos.x - GetPosition().x;
                        dy = targetPos.y - GetPosition().y;

                        angle = Math::Atan2(dy, dx);
                        // Ajustar para intervalo [0, 2*pi)
                        if (angle < 0)
                            angle += 2 * Math::Pi;

                        SetRotation(angle);
                        // SDL_Log("%f", GetRotation());
                        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
                        break;
                }
                // SDL_Log("Fora do múltiplo de 3 mJumoCount: %d", mJumpCount);
            }

            // switch (mWichWallFrogIs) {
            //     case 0:
            //         mRigidBodyComponent->SetVelocity(Vector2(900, -600));
            //         break;
            //     case 1:
            //         mRigidBodyComponent->SetVelocity(Vector2(-900, 600));
            //         break;
            //     case 2:
            //         mRigidBodyComponent->SetVelocity(Vector2(-900, -600));
            //         break;
            //     case 3:
            //         mRigidBodyComponent->SetVelocity(Vector2(900, 600));
            //         break;
            // }

            // mRigidBodyComponent->SetVelocity(Vector2(100, -700));

            // Player *player = GetGame()->GetPlayer();
            // float dx = player->GetPosition().x - GetPosition().x;
            // float dy = player->GetPosition().y - GetPosition().y;
            //
            // float angle = Math::Atan2(dy, dx);
            // // Ajustar para intervalo [0, 2*pi)
            // if (angle < 0)
            //     angle += 2 * Math::Pi;
            //
            // SetRotation(angle);
            //
            // if (mKnockBackTimer >= mKnockBackDuration) {
            //     mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mJumpForce));
            // }
        }
        else {
            // SDL_Log("Dentro do else, ou seja, se mTimerBetweenJumps < mDurationBetween mJumoCount: %d", mJumpCount);
            mRigidBodyComponent->SetVelocity(Vector2::Zero);

        }
    }
}

void Frog::Tongue(float delTime) {
    if (!mIsLicking) {
        mState = FrogState::frogStop;
        return;
    }

    mTongue->SetState(ActorState::Active);
    mTongue->SetRotation(GetRotation());


    // mTongueTimer += delTime;
    // if (mTongueTimer >= mTongueDuration) {
    //     mTongueTimer -= mTongueDuration;
    //     mState = FrogState::frogStop;
    // }
}


void Frog::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mWalkingAroundMoveSpeed = mWalkingAroundMoveSpeed / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;
    mArenaMinPos.x = mArenaMinPos.x / oldScale * newScale;
    mArenaMinPos.y = mArenaMinPos.y / oldScale * newScale;
    mArenaMaxPos.x = mArenaMaxPos.x / oldScale * newScale;
    mArenaMaxPos.y = mArenaMaxPos.y / oldScale * newScale;
    mMinDistFromEdge = mMinDistFromEdge / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth * 1.3f);
    mDrawAnimatedComponent->SetHeight(mHeight * 1.3f);

    Vector2 v1(-mHeight / 2, -mHeight / 2);
    Vector2 v2(mHeight / 2, -mHeight / 2);
    Vector2 v3(mHeight / 2, mHeight / 2);
    Vector2 v4(-mHeight / 2, mHeight / 2);

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
