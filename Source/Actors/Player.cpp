//
// Created by roger on 22/04/2025.
//

#include "Player.h"

#include "../Game.h"
#include "../Actors/Sword.h"
#include "../Actors/FireBall.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DashComponent.h"

Player::Player(Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mIsOnGround(false)

    ,mIsJumping(false)
    ,mCanJump(true)
    ,mJumpTime(0.0f)
    ,mMaxJumpTime(0.25f)
    ,mJumpForce(-850.0f)
    ,mMoveSpeed(550)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)

    ,mPrevSwordPressed(false)
    ,mSwordCooldownTimer(0.3f)
    ,mSwordCooldownDuration(0.3f)
    ,mSwordDirection(0)

    ,mPrevFireBallPressed(false)
    ,mFireBallCooldownTimer(1.0f)
    ,mFireBallCooldownDuration(1.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallTimer(0.0f)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mFireballRecoil(0.0f)

    ,mCanWallSlide(true)
    ,mIsWallSliding(false)
    ,mWallSlideSide(WallSlideSide::notSliding)
    ,mWallSlideSpeed(300)
    ,mTryingLeavingWallSlideLeft(0)
    ,mTryingLeavingWallSlideRight(0)
    ,mTimerToLeaveWallSlidingLeft(0.15)
    ,mTimerToLeaveWallSlidingRight(0.15)
    ,mMaxTimerToLiveWallSliding(0.15)

    ,mWallJumpTimer(0.15)
    ,mWallJumpMaxTime(0.15)
{
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mDrawComponent = new DrawComponent(this, vertices);
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1300);
    mAABBComponent = new AABBComponent(this, v1, v3, {255, 255, 0, 255});
    mDashComponent = new DashComponent(this, 1400, 0.18f, 0.5f);

    mSword = new Sword(game, 60, 20, 0.1f);
}

void Player::OnProcessInput(const uint8_t* state) {
    mTryingLeavingWallSlideLeft = 0;
    mTryingLeavingWallSlideRight = 0;

    if (!(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT]) && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    else {
        if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }

        if (state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }
    }

    if (!(state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_UP])) {
        mSwordDirection = GetRotation();
    }
    else {
        if (state[SDL_SCANCODE_DOWN]) {
            mSwordDirection = Math::Pi / 2;
            GetGame()->GetCamera()->mLookDown = true;
        }
        if (state[SDL_SCANCODE_UP]) {
            mSwordDirection = 3 * Math::Pi / 2;
            GetGame()->GetCamera()->mLookUp = true;
        }
    }

    //Início do pulo
    if (state[SDL_SCANCODE_Z] && !mIsFireAttacking) {
        if (!mDashComponent->GetIsDashing()) {
            // Pulo do chao
            if (mIsOnGround && !mIsJumping && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(0, mJumpForce));
                mIsJumping = true;
                mCanJump = false;
                mJumpTime = 0.0f;
            }
            // Wall jumping
            if (mIsWallSliding && !mIsJumping && mCanJump) {
                if (mWallSlideSide == WallSlideSide::left) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mJumpForce));
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mJumpForce));
                }
                mIsJumping = true;
                mCanJump = false;
                mJumpTime = 0.0f;
                mWallJumpTimer = 0;
            }
            // Pulo no ar
            if (!(mIsOnGround || mIsWallSliding) && mJumpCountInAir < mMaxJumpsInAir && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(0, mJumpForce));
                mIsJumping = true;
                mCanJump = false;
                mJumpTime = 0.0f;
                mJumpCountInAir++; // Incrementa número de pulos
            }
        }
    }
    else {
        mIsJumping = false; // jogador soltou o W
        mCanJump = true;
    }

    // Dash
    if (state[SDL_SCANCODE_C] && !mIsFireAttacking) {
        mDashComponent->UseDash(mIsOnGround);
    }

    // Sword
    // Detecta borda de descida da tecla K e cooldown pronto
    if (state[SDL_SCANCODE_X] && !mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration) {
        // Ativa a espada
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(mSwordDirection);
        mSword->SetPosition(GetPosition());

        // Inicia cooldown
        mSwordCooldownTimer = 0;
    }
    mPrevSwordPressed = state[SDL_SCANCODE_X];

    // FireBall
    if (state[SDL_SCANCODE_A] && !mPrevFireBallPressed && mFireBallCooldownTimer >= mFireBallCooldownDuration) {
        std::vector<FireBall*> fireBalls = GetGame()->GetFireBalls();
        for (FireBall* f : fireBalls) {
            if (f->GetState() == ActorState::Paused) {
                f->SetState(ActorState::Active);
                f->SetRotation(GetRotation());
                f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                mIsFireAttacking = true;
                mStopInAirFireBallTimer = 0;
                break;
            }
        }
        // Inicia cooldown
        mFireBallCooldownTimer = 0;
    }
    mPrevFireBallPressed = state[SDL_SCANCODE_A];

}

void Player::OnUpdate(float deltaTime)
{
    if (mSwordCooldownTimer < mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
    }

    if (mStopInAirFireBallTimer < mStopInAirFireBallMaxDuration) {
        mStopInAirFireBallTimer += deltaTime;
    }
    else {
        mIsFireAttacking = false;
    }

    mTimerToLeaveWallSlidingLeft += mTryingLeavingWallSlideLeft * deltaTime;
    mTimerToLeaveWallSlidingRight += mTryingLeavingWallSlideRight * deltaTime;

    mWallJumpTimer += deltaTime;

    mIsOnGround = false;
    mIsWallSliding = false;
    mWallSlideSide = WallSlideSide::notSliding;

    if (mIsFireAttacking) {
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mFireballRecoil, 0));
    }

    if (mIsJumping) {
        mJumpTime += deltaTime;
        if (mJumpTime <= mMaxJumpTime) {
            // Gravidade menor
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 100 * deltaTime));
            }
        } else {
            mIsJumping = false;
            // Gravidade
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
            }
        }
    }
    else {
        // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
        }
    }

    // Colisao com ground
    std::array<bool, 4> collisionSide;
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                collisionSide = mAABBComponent->ResolveColision(*g->GetComponent<AABBComponent>());
            }
            else {
                collisionSide[0] = false;
                collisionSide[1] = false;
                collisionSide[2] = false;
                collisionSide[3] = false;
            }
            // colidiu top
            if (collisionSide[0]) {
                if (mRigidBodyComponent->GetVelocity().y >= 0) {
                    mIsOnGround = true;
                    mIsJumping  = false;
                    // Resetar dash no ar
                    mDashComponent->SetHasDashedInAir(false);
                    // RESET DO CONTADOR DE PULO
                    mJumpCountInAir = 0;
                }
            }

            // colidiu bot
            if (collisionSide[1]) {
                mJumpTime = mMaxJumpTime;
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, 1));
            }

            //colidiu pelas laterias
            if (mCanWallSlide) {
                if ((collisionSide[2] || collisionSide[3])) {
                    mIsWallSliding = true;
                    if (collisionSide[2]) {
                        mWallSlideSide = WallSlideSide::left;
                        SetRotation(Math::Pi);
                    }
                    else {
                        mWallSlideSide = WallSlideSide::right;
                        SetRotation(0);
                    }
                    if (mRigidBodyComponent->GetVelocity().y >= 0) {
                        mIsJumping  = false;
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mWallSlideSpeed));
                    }
                }
            }
        }
    }
}
