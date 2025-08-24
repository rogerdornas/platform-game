//
// Created by roger on 22/04/2025.
//

#include "Player.h"
#include <cfloat>
#include "Checkpoint.h"
#include "Effect.h"
#include "HookPoint.h"
#include "../Game.h"
#include "../Actors/Sword.h"
#include "../Actors/FireBall.h"
#include "../Actors/ParticleSystem.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DashComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawRopeComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Player::Player(Game* game, float width, float height)
    :Actor(game)
    ,mStartingPosition(Vector2::Zero)
    ,mHeight(height * mGame->GetScale())
    ,mWidth(width * mGame->GetScale())

    ,mIsOnGround(false)
    ,mIsOnMovingGround(false)
    ,mMovingGroundVelocity(Vector2::Zero)
    ,mMoveSpeed(700 * mGame->GetScale())

    ,mIsJumping(false)
    ,mJumpTimer(0.0f)
    ,mMaxJumpTime(0.25f)
    ,mJumpForce(-900.0f * mGame->GetScale())
    ,mCanJump(true)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)
    ,mLowGravity(50.0f * mGame->GetScale())
    ,mMediumGravity(3000.0f * mGame->GetScale())
    ,mHighGravity(4500.0f * mGame->GetScale())

    ,mCanDash(true)

    ,mPrevSwordPressed(false)
    ,mSwordCooldownTimer(0.0f)
    ,mSwordCooldownDuration(0.4f)
    ,mSwordWidth(mWidth * 3.0f)
    ,mSwordHeight(mHeight * 1.3f)
    ,mSwordDamage(10.0f)
    ,mSwordDirection(0)
    ,mSwordHitEnemy(false)
    ,mSwordHitGround(false)
    ,mSwordHitSpike(false)

    ,mCanFireBall(true)
    ,mPrevFireBallPressed(false)
    ,mFireBallCooldownDuration(0.1f)
    ,mFireBallCooldownTimer(0.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallTimer(0.0f)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mFireballRecoil(0.0f * mGame->GetScale())
    ,mFireballWidth(50 * mGame->GetScale())
    ,mFireBallHeight(50 * mGame->GetScale())
    ,mFireballSpeed(1500 * mGame->GetScale())
    ,mFireballDamage(20.0f)
    ,mMaxMana(90.0f)
    ,mMana(90.0f)
    ,mManaIncreaseRate(6.0f)
    ,mFireballManaCost(30.0f)

    ,mCanWallSlide(true)
    ,mIsWallSliding(false)
    ,mWallSlideSide(WallSlideSide::notSliding)
    ,mWallSlideSpeed(300 * mGame->GetScale())
    ,mTryingLeavingWallSlideLeft(0)
    ,mTryingLeavingWallSlideRight(0)
    ,mTimerToLeaveWallSlidingLeft(0.15f)
    ,mTimerToLeaveWallSlidingRight(0.15f)
    ,mMaxTimerToLiveWallSliding(0.15f)

    ,mWallJumpTimer(0.15f)
    ,mWallJumpMaxTime(0.15f)

    ,mKnockBackSpeed(1000.0f * mGame->GetScale())
    ,mKnockBackTimer(0.0f)
    ,mKnockBackDuration(0.2f)
    ,mCameraShakeStrength(70.0f * mGame->GetScale())

    ,mMaxHealthPoints(70.0f)
    ,mHealthPoints(mMaxHealthPoints)
    ,mIsInvulnerable(false)
    ,mInvulnerableDuration(0.8f)
    ,mInvulnerableTimer(mInvulnerableDuration)
    ,mMaxHealCount(3)
    ,mHealCount(mMaxHealCount)
    ,mHealAmount(30.0f)
    ,mIsHealing(false)
    ,mHealAnimationDuration(0.8f)
    ,mHealAnimationTimer(0.0f)

    ,mMoney(1000)
    ,mStartMoney(0)

    ,mIsHooking(false)
    ,mPrevHookPressed(false)
    ,mHookDirection(Vector2::Zero)
    ,mHookSpeed(1600.0f * mGame->GetScale())
    ,mHookCooldownDuration(0.4f)
    ,mHookCooldownTimer(0.0f)
    ,mHookingDuration(0.15f)
    ,mHookingTimer(0.0f)
    ,mHookEnd(Vector2::Zero)
    ,mHookAnimProgress(0.0f)
    ,mIsHookAnimating(false)
    ,mHookPoint(nullptr)
    ,mHookAnimationDuration(0.2f)
    ,mHookSegments(20)
    ,mHookAmplitude(12.0f * mGame->GetScale())
    ,mHookSegmentHeight(8.0f * mGame->GetScale())

    ,mIsRunning(false)
    ,mHurtDuration(0.2f)
    ,mHurtTimer(mHurtDuration)
    ,mBlink(false)
    ,mBlinkDuration(0.03f)
    ,mBlinkTimer(mBlinkDuration)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
    ,mWasOnGround(false)
    ,mDeathCounter(0)
    ,mDeathAnimationDuration(1.0f)
    ,mDeathAnimationTimer(0.0f)
    ,mIsDead(false)

    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
    ,mDrawRopeComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // Esquilo estático
    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Esquilo/zenzen.png", 100, 100, 1000);

    // Raposa animada
    // mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.3, 0.91 * mWidth * 2.3, "../Assets/Sprites/Raposa 2/Raposa.png", "../Assets/Sprites/Raposa 2/Raposa.json", 1000);
    //
    // std::vector<int> idle = {2};
    // mDrawAnimatedComponent->AddAnimation("idle", idle);
    //
    // std::vector<int> run = {3, 4, 5, 6, 7};
    // mDrawAnimatedComponent->AddAnimation("run", run);
    //
    // std::vector<int> hitted = {1};
    // mDrawAnimatedComponent->AddAnimation("hitted", hitted);
    //
    // std::vector<int> dash = {0};
    // mDrawAnimatedComponent->AddAnimation("dash", dash);
    //
    //
    // mDrawAnimatedComponent->SetAnimation("idle");
    // mDrawAnimatedComponent->SetAnimFPS(16.0f);

    // Esquilo animado
    // mDrawAnimatedComponent = new DrawAnimatedComponent(this, 120, 120, "../Assets/Sprites/Esquilo/Esquilo.png", "../Assets/Sprites/Esquilo/Esquilo.json", 1000);
    // mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.5, mWidth * 2.5,
    //                                                    "../Assets/Sprites/Esquilo/Esquilo.png",
    //                                                    "../Assets/Sprites/Esquilo/Esquilo.json", 1000);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 3.6f, mWidth * 3.6f,
                                                   "../Assets/Sprites/Esquilo3/Esquilo.png",
                                                   "../Assets/Sprites/Esquilo3/Esquilo.json", 1000);

    std::vector idle = {19};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector attackFront = {19, 2, 3};
    mDrawAnimatedComponent->AddAnimation("attackFront", attackFront);

    std::vector attackUp = {19, 4, 5};
    mDrawAnimatedComponent->AddAnimation("attackUp", attackUp);

    std::vector attackDown = {19, 0, 1};
    mDrawAnimatedComponent->AddAnimation("attackDown", attackDown);

    std::vector dash = {6, 7, 7, 7, 8};
    mDrawAnimatedComponent->AddAnimation("dash", dash);

    std::vector run = {23, 24, 25, 26, 27, 28};
    mDrawAnimatedComponent->AddAnimation("run", run);

    std::vector heal = {12, 13, 14, 15, 16, 16, 15, 14, 13, 12};
    mDrawAnimatedComponent->AddAnimation("heal", heal);

    std::vector wallSlide = {29};
    mDrawAnimatedComponent->AddAnimation("wallSlide", wallSlide);

    std::vector hurt = {17, 18};
    mDrawAnimatedComponent->AddAnimation("hurt", hurt);

    std::vector die = {17, 9, 10, 11, 11, 11};
    mDrawAnimatedComponent->AddAnimation("die", die);

    std::vector jumpUp = {20};
    mDrawAnimatedComponent->AddAnimation("jumpUp", jumpUp);

    std::vector jumpApex = {21};
    mDrawAnimatedComponent->AddAnimation("jumpApex", jumpApex);

    std::vector falling = {22};
    mDrawAnimatedComponent->AddAnimation("falling", falling);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);

    mDrawRopeComponent = new DrawRopeComponent(this, "../Assets/Sprites/Rope/Rope2.png");
    mDrawRopeComponent->SetNumSegments(mHookSegments);
    mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {255, 255, 0, 255});
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 1600 * mGame->GetScale());
    mAABBComponent = new AABBComponent(this, v1, v3);
    mDashComponent = new DashComponent(this, 1500 * mGame->GetScale(), 0.2f, 0.5f);

    mSword = new Sword(mGame, this, mSwordWidth, mSwordHeight, 0.15f, mSwordDamage);
}

void Player::OnProcessInput(const uint8_t* state, SDL_GameController &controller) {
    mTryingLeavingWallSlideLeft = 0;
    mTryingLeavingWallSlideRight = 0;
    mIsRunning = false;

    bool left = (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_LCTRL]) ||
                SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -20000;

    bool leftSlow = (state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_LCTRL]) ||
                    (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -10000 &&
                     SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > -20000);

    bool right = (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LCTRL]) ||
                 SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) ||
                 SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 20000;

    bool rightSlow = (state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LCTRL]) ||
                     (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 10000 &&
                      SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < 20000);

    bool lookUp = (state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_LCTRL]) ||
                   SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) < -28000;

    bool lodDown = (state[SDL_SCANCODE_DOWN] && state[SDL_SCANCODE_LCTRL]) ||
                    SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) > 28000;

    bool up = state[SDL_SCANCODE_UP] ||
              SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_UP) ||
              SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) < -22000;

    bool down = state[SDL_SCANCODE_DOWN] ||
                SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) > 22000;

    bool jump = state[SDL_SCANCODE_Z] ||
                SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_A);

    bool dash = state[SDL_SCANCODE_C] ||
                SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    bool sword = state[SDL_SCANCODE_X] ||
                 SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_X);

    bool fireBall = state[SDL_SCANCODE_A] ||
                    SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_B);

    bool heal = state[SDL_SCANCODE_V] ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 10000;

    bool hook = state[SDL_SCANCODE_S] ||
            SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);

    if (!left && !leftSlow && !right && !rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
        !mIsOnMovingGround && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration) && (mHookingTimer >= mHookingDuration * 2)) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    else {
        if (left && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) &&
            (mKnockBackTimer >= mKnockBackDuration) && (mHookingTimer >= mHookingDuration * 2)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (leftSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) &&
            (mKnockBackTimer >= mKnockBackDuration) && (mHookingTimer >= mHookingDuration * 2)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (right && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)
            && (mKnockBackTimer >= mKnockBackDuration) && (mHookingTimer >= mHookingDuration * 2)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }

        if (rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime)
            && (mKnockBackTimer >= mKnockBackDuration) && (mHookingTimer >= mHookingDuration * 2)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                             mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                         mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }
    }

    if (lookUp) {
        mGame->GetCamera()->SetLookUp();
    }

    if (lodDown) {
        mGame->GetCamera()->SetLookDown();
    }

    if (!down && !up) {
        mSwordDirection = GetRotation();
    }
    else {
        if (down) {
            mSwordDirection = Math::Pi / 2;
        }
        if (up) {
            mSwordDirection = 3 * Math::Pi / 2;
        }
    }

    //Início do pulo
    if (jump && !mIsFireAttacking) {
        if (!mDashComponent->GetIsDashing()) {
            // Pulo do chao
            if (mIsOnGround && !mIsJumping && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce)
                                                 + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
            }
            // Wall jumping
            if (mIsWallSliding && !mIsJumping && mCanJump) {
                if (mWallSlideSide == WallSlideSide::left) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                    SetRotation(Math::Pi);
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                    SetRotation(0);
                }

                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mWallJumpTimer = 0;
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
            }
            // Pulo no ar
            if (!(mIsOnGround || mIsWallSliding) && mJumpCountInAir < mMaxJumpsInAir && mCanJump
                && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce)
                                                 + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mJumpCountInAir++; // Incrementa número de pulos
                mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
            }
        }
    }
    else {
        mIsJumping = false; // jogador soltou o W
        mCanJump = true;
    }

    // Dash
    if (mCanDash) {
        if (dash && !mIsFireAttacking) {
            mDashComponent->UseDash(mIsOnGround);
            mIsHooking = false;
            mHookAnimProgress = 1.0f;
            mIsHookAnimating = false;
            mHookPoint = nullptr;
            mDrawRopeComponent->SetIsVisible(false);
        }
    }

    // Sword
    // Detecta borda de descida da tecla K e cooldown pronto
    if (sword && !mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration) {
        mDrawAnimatedComponent->ResetAnimationTimer();
        mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
        // Ativa a espada
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(mSwordDirection);
        mSword->SetPosition(GetPosition());
        mSwordHitEnemy = false;
        mSwordHitGround = false;
        mSwordHitSpike = false;

        // Inicia cooldown
        mSwordCooldownTimer = 0;
    }
    mPrevSwordPressed = sword;

    // FireBall
    if (mCanFireBall) {
        if (fireBall &&
            !mPrevFireBallPressed &&
            mFireBallCooldownTimer >= mFireBallCooldownDuration &&
            mMana >= mFireballManaCost)
        {
            std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
            for (FireBall* f: fireBalls) {
                if (f->GetState() == ActorState::Paused) {
                    f->SetState(ActorState::Active);
                    f->SetRotation(GetRotation());
                    f->SetWidth(mFireballWidth);
                    f->SetHeight(mFireBallHeight);
                    f->SetSpeed(mFireballSpeed);
                    f->SetDamage(mFireballDamage);
                    f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                    mIsFireAttacking = true;
                    mStopInAirFireBallTimer = 0;
                    mMana -= mFireballManaCost;
                    break;
                }
            }
            // Inicia cooldown
            mFireBallCooldownTimer = 0;
        }
        mPrevFireBallPressed = fireBall;
    }

    // Heal
    if (heal && mHealCount > 0 && mHealthPoints < mMaxHealthPoints && mIsOnGround) {
        if (left || leftSlow || right || rightSlow || jump || dash || sword || fireBall) {
            mHealAnimationTimer = 0;
        }
        else {
            mIsHealing = true;
            if (mHealAnimationTimer >= mHealAnimationDuration) {
                mHealAnimationTimer = 0;
                mHealthPoints += mHealAmount;
                mHealCount--;
                if (mHealthPoints > mMaxHealthPoints) {
                    mHealthPoints = mMaxHealthPoints;
                }
            }
        }
    }
    else {
        mIsHealing = false;
        mHealAnimationTimer = 0;
    }

    // Hook
    std::vector<HookPoint* > hookPoints = mGame->GetHookPoints();

    HookPoint* nearestHookPoint = nullptr;
    float nearestDistance = FLT_MAX;

    for (HookPoint* hp: hookPoints) {
        float dist = (GetPosition() - hp->GetPosition()).Length();
        if (dist < hp->GetRadius()) {
            float distX = GetPosition().x - hp->GetPosition().x;

            // Verifica se o jogador está olhando para a direção do hookPoint
            bool lookingRight = GetRotation() == 0 && distX < 0;
            bool lookingLeft = GetRotation() == Math::Pi && distX > 0;

            if ((lookingRight || lookingLeft) && dist < nearestDistance) {
                nearestDistance = dist;
                nearestHookPoint = hp;
            }
        }
    }
    if (nearestHookPoint && (nearestHookPoint != mHookPoint)) {
        nearestHookPoint->SetHookPointState(HookPoint::HookPointState::InRange);
    }

    if (nearestHookPoint &&
        hook &&
        !mPrevHookPressed &&
        !mDashComponent->GetIsDashing() &&
        mHookCooldownTimer >= mHookCooldownDuration)
    {
        mHookPoint = nearestHookPoint;
        nearestHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
        Vector2 dir = (nearestHookPoint->GetPosition() - GetPosition());
        if (dir.Length() > 0) {
            dir.Normalize();
        }
        mHookDirection = dir;
        mIsHooking = true;
        mHookCooldownTimer = 0.0f;
        mHookingTimer = 0.0f;

        // Quando hook começa
        mHookEnd = nearestHookPoint->GetPosition();
        mHookAnimProgress = 0.0f;
        mIsHookAnimating = true;
        mDrawRopeComponent->SetIsVisible(true);

        // Resetar dash no ar
        mDashComponent->SetHasDashedInAir(false);
        // RESET DO CONTADOR DE PULO
        mJumpCountInAir = 0;

        mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
        mDrawRopeComponent->SetAnimationProgress(mHookAnimProgress);
    }
    mPrevHookPressed = hook;
}

void Player::OnUpdate(float deltaTime) {
    if (mSwordCooldownTimer < mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
    }

    if (mHookCooldownTimer < mHookCooldownDuration) {
        mHookCooldownTimer += deltaTime;
    }

    if (mHookingTimer < mHookingDuration * 5) {
        mHookingTimer += deltaTime;
    }

    if (mIsHookAnimating) {
        mHookAnimProgress += deltaTime / mHookAnimationDuration;
        if (mHookAnimProgress >= 1.0f) {
            mHookAnimProgress = 1.0f;
            mIsHookAnimating = false;
            mHookPoint = nullptr;
            mDrawRopeComponent->SetIsVisible(false);
        }
        mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
        mDrawRopeComponent->SetAnimationProgress(mHookAnimProgress);
        if (mHookPoint) {
            mHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
        }
    }

    if (mKnockBackTimer < mKnockBackDuration) {
        mKnockBackTimer += deltaTime;
    }

    if (mIsDead) {
        mDeathAnimationTimer += deltaTime;
    }

    if (mStopInAirFireBallTimer < mStopInAirFireBallMaxDuration) {
        mStopInAirFireBallTimer += deltaTime;
    }
    else {
        mIsFireAttacking = false;
    }

    if (mInvulnerableTimer < mInvulnerableDuration) {
        mInvulnerableTimer += deltaTime;
        mIsInvulnerable = true;
    }
    else {
        if (mGame->GetGamePlayState() != Game::GamePlayState::Cutscene) {
            mIsInvulnerable = false;
        }
    }

    if (mMana < mMaxMana) {
        mMana += mManaIncreaseRate * deltaTime;
        if (mMana > mMaxMana) {
            mMana = mMaxMana;
        }
    }

    if (mIsHealing) {
        if (mHealAnimationTimer == 0) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mHealAnimationTimer += deltaTime;
    }

    mTimerToLeaveWallSlidingLeft += mTryingLeavingWallSlideLeft * deltaTime;
    mTimerToLeaveWallSlidingRight += mTryingLeavingWallSlideRight * deltaTime;

    mWallJumpTimer += deltaTime;

    // Controla animação de levar dano
    if (mHurtTimer < mHurtDuration) {
        mHurtTimer += deltaTime;
    }
    else {
        if (mIsInvulnerable) {
            mBlinkTimer += deltaTime;
            if (mBlinkTimer >= mBlinkDuration) {
                mBlink = !mBlink;
                mBlinkTimer -= mBlinkDuration;
            }
        }
    }

    mIsOnGround = false;
    mIsOnMovingGround = false;
    mMovingGroundVelocity = Vector2::Zero;
    mIsWallSliding = false;
    mWallSlideSide = WallSlideSide::notSliding;

    if (mIsFireAttacking) {
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mFireballRecoil, 0) + mMovingGroundVelocity);
    }

    if (mIsJumping) {
        mJumpTimer += deltaTime;
        if (mJumpTimer <= mMaxJumpTime) {
            // Gravidade menor
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround && !mIsHooking) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mLowGravity * deltaTime));
            }
        }
        else {
            mIsJumping = false;
        }
    }
    else {
        // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround && !mIsHooking) {
            if (mRigidBodyComponent->GetVelocity().y < 0) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mMediumGravity * deltaTime));
            }
            else {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                         mRigidBodyComponent->GetVelocity().y
                                                         + mHighGravity * deltaTime));
            }
        }
    }

    ResolveEnemyCollision();
    ResolveGroundCollision();

    if (mIsHooking) {
        if (mHookingTimer < mHookingDuration) {
            mRigidBodyComponent->SetVelocity(mHookDirection * mHookSpeed);
        } else {
            mIsHooking = false;
        }
    }

    if (mIsRunning && mIsOnGround) {
        mRunningSoundIntervalTimer += deltaTime;
        if (mRunningSoundIntervalTimer >= mRunningSoundIntervalDuration) {
            mRunningSoundIntervalTimer -= mRunningSoundIntervalDuration;
            mGame->GetAudio()->PlayVariantSound("StepsInGrass/StepsInGrass.wav", 4);
        }
    }

    if (mWasOnGround == false) {
        if (mIsOnGround) {
            mGame->GetAudio()->PlaySound("FallOnGround.wav");
            mRunningSoundIntervalTimer = 0;
        }
    }

    mWasOnGround = mIsOnGround;

    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000 * mGame->GetScale()) {
        SetPosition(mStartingPosition);
    }

    if (Died()) {
        mGame->SetGamePlayState(Game::GamePlayState::GameOver);
        mAABBComponent->SetActive(false);
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mKnockBackTimer = mKnockBackDuration;
        mInvulnerableTimer = mInvulnerableDuration;
        mIsHealing = false;
        mGame->GetAudio()->StopAllSounds();
        if (mDeathAnimationTimer >= mDeathAnimationDuration) {
            mDeathCounter++;
            mDeathAnimationTimer = 0;
            mGame->SetResetLevel();
            SetState(ActorState::Paused);
        }
    }

    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }
}

void Player::ResolveGroundCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground* > grounds = mGame->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                }
                else {
                    collisionNormal = Vector2::Zero;
                }

                // colidiu top
                if (collisionNormal == Vector2::NegUnitY) {
                    mIsOnGround = true;
                    mIsJumping = false;
                    // Resetar dash no ar
                    mDashComponent->SetHasDashedInAir(false);
                    // RESET DO CONTADOR DE PULO
                    mJumpCountInAir = 0;
                    // Move o player junto ao ground em movimento
                    if (g->GetIsMoving()) {
                        mIsOnMovingGround = true;
                        mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                        if (!mDashComponent->GetIsDashing()) {
                            mRigidBodyComponent->SetVelocity(mMovingGroundVelocity);
                            SetPosition(Vector2(GetPosition().x,
                                                g->GetPosition().y - g->GetHeight() / 2 - mHeight / 2 + 1));
                            // Gambiarra (Ao detectar colisão por cima em moving ground, desce o ‘player’ 1 pixel para baixo para não ficar trepidando
                        }
                    }
                }

                // colidiu bot
                if (collisionNormal == Vector2::UnitY) {
                    mJumpTimer = mMaxJumpTime;
                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, 1));
                    if (g->GetIsMoving()) {
                        if (g->GetComponent<RigidBodyComponent>()->GetVelocity().y > 0) {
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                                     g->GetComponent<RigidBodyComponent>()->
                                                                        GetVelocity().y * 1.5));
                            // Para não grudar quando pular por baixo de uma plataforma movel
                        }
                    }
                }

                //colidiu pelas laterais
                if (mCanWallSlide) {
                    if ((collisionNormal == Vector2::NegUnitX || collisionNormal == Vector2::UnitX)) {
                        // Move o player junto ao ground em movimento
                        if (g->GetIsMoving()) {
                            mIsOnMovingGround = true;
                            mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                        }

                        mIsWallSliding = true;
                        // Testa se não está dashando para não bugar quando dar um dash na quina de baixo e inverter a direção do dash
                        if (collisionNormal == Vector2::NegUnitX && !mDashComponent->GetIsDashing()) {
                            mWallSlideSide = WallSlideSide::left;
                            if (!mIsOnGround) {
                                // SetRotation(Math::Pi);
                            }
                            if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(
                                    mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x + 100,
                                    mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + 100,
                                                                         mRigidBodyComponent->GetVelocity().y));
                            }
                        }
                        else if (collisionNormal == Vector2::UnitX && !mDashComponent->GetIsDashing()) {
                            mWallSlideSide = WallSlideSide::right;
                            if (!mIsOnGround) {
                                // SetRotation(0);
                            }
                            if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(
                                    mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x - 100,
                                    mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x - 100,
                                                                         mRigidBodyComponent->GetVelocity().y));
                            }
                        }
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                        if (mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                            mIsJumping = false;
                            mRigidBodyComponent->SetVelocity(Vector2(
                                mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x,
                                mWallSlideSpeed + mMovingGroundVelocity.y));
                        }
                        else if (!mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y > 0) {
                            mIsJumping = false;
                            mRigidBodyComponent->SetVelocity(
                                Vector2(mRigidBodyComponent->GetVelocity().x, mWallSlideSpeed));
                        }
                    }
                }

                if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) {
                    // Colisão da sword com grounds
                    if (!mSwordHitGround) {
                        collisionNormal = mSword->GetComponent<ColliderComponent>()->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
                            auto* grass = new ParticleSystem(mGame, 6, 150.0, 0.30, 0.05f);
                            if (collisionNormal == Vector2::NegUnitY) {
                                grass->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y - g->GetHeight() / 2));
                            }
                            if (collisionNormal == Vector2::UnitY) {
                                grass->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y + g->GetHeight() / 2));
                            }
                            if (collisionNormal == Vector2::NegUnitX) {
                                grass->SetPosition(Vector2(g->GetPosition().x - g->GetWidth() / 2, GetPosition().y));
                            }
                            if (collisionNormal == Vector2::UnitX) {
                                grass->SetPosition(Vector2(g->GetPosition().x + g->GetWidth() / 2, GetPosition().y));
                            }

                            grass->SetEmitDirection(mSword->GetForward() * -1);
                            grass->SetIsSplash(true);
                            grass->SetParticleSpeedScale(0.5);
                            SDL_Color color;
                            switch (mGame->GetGameScene()) {
                                case Game::GameScene::LevelTeste:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Coliseu:
                                    color = {102, 114, 145, 255};
                                    break;

                                case Game::GameScene::Level1:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Level2:
                                    color = {102, 114, 145, 255};
                                    break;

                                case Game::GameScene::Level3:
                                    color = {80, 148, 45, 255};
                                    break;

                                case Game::GameScene::Level4:
                                    color = {98, 171, 212, 255};
                                    break;

                                case Game::GameScene::Level5:
                                    color = {98, 171, 212, 255};
                                    break;

                                default:
                                    color = {80, 148, 45, 255};
                                    break;
                            }
                            grass->SetParticleColor(color);
                            grass->SetParticleGravity(true);
                            mSwordHitGround = true;
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    // SetPosition(mStartingPosition);

                    // mGame->mResetLevel = true;

                    // mRigidBodyComponent->SetVelocity(Vector2::Zero);
                    // SetPosition(g->GetRespawPosition());
                    // ReceiveHit(10, Vector2::Zero);

                    collisionNormal = mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());

                    mDashComponent->StopDash();

                    ReceiveHit(10, collisionNormal);
                }
                else if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) { // Colisão da sword com spikes
                    if (!mSwordHitSpike) {
                        collisionNormal = mSword->GetComponent<ColliderComponent>()->CollisionSide(*g->GetComponent<ColliderComponent>());
                        if ((collisionNormal == Vector2::NegUnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::UnitY && Math::Abs(mSword->GetForward().y) == 1) ||
                            (collisionNormal == Vector2::NegUnitX && Math::Abs(mSword->GetForward().x) == 1) ||
                            (collisionNormal == Vector2::UnitX && Math::Abs(mSword->GetForward().x) == 1) )
                        {
                            mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
                            for (int i = 0; i < 3; i++) {
                                auto* sparkEffect = new Effect(mGame);
                                sparkEffect->SetDuration(0.1f);

                                // collisionSide = mSword->GetComponent<AABBComponent>()->CollisionSide(*g->GetComponent<AABBComponent>());
                                if (collisionNormal == Vector2::NegUnitY) {
                                    sparkEffect->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y - g->GetHeight() / 2));
                                }

                                if (collisionNormal == Vector2::UnitY) {
                                    sparkEffect->SetPosition(Vector2(mSword->GetPosition().x, g->GetPosition().y + g->GetHeight() / 2));
                                }

                                if (collisionNormal == Vector2::NegUnitX) {
                                    sparkEffect->SetPosition(Vector2(g->GetPosition().x - g->GetWidth() / 2, GetPosition().y));
                                }

                                if (collisionNormal == Vector2::UnitX) {
                                    sparkEffect->SetPosition(Vector2(g->GetPosition().x + g->GetWidth() / 2, GetPosition().y));
                                }

                                sparkEffect->SetEffect(TargetEffect::SwordHit);
                            }
                            mSwordHitSpike = true;
                            mSwordHitGround = true;
                        }
                    }

                    if (mSword->GetRotation() == Math::Pi / 2) {
                        if (!mDashComponent->GetIsDashing()) {
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                        }
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                    }
                }
            }
        }
    }
}

void Player::ResolveEnemyCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        bool swordHitEnemy = false;
        for (Enemy* e: enemies) {
            if (mAABBComponent->Intersect(*e->GetComponent<ColliderComponent>())) {
                collisionNormal = mAABBComponent->ResolveCollision(*e->GetComponent<ColliderComponent>());

                mDashComponent->StopDash();

                ReceiveHit(e->GetContactDamage(), collisionNormal);
            }

            if (mSword->GetComponent<ColliderComponent>()->Intersect(*e->GetComponent<ColliderComponent>())) { // Colisão da sword com enemies
                if (!mSwordHitEnemy) {
                    e->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
                    swordHitEnemy = true;
                }
                if (mSword->GetRotation() == Math::Pi / 2) {
                    if (!mDashComponent->GetIsDashing())
                        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                    // Resetar dash no ar
                    mDashComponent->SetHasDashedInAir(false);
                    // RESET DO CONTADOR DE PULO
                    mJumpCountInAir = 0;
                }
            }
        }
        if (swordHitEnemy) {
            mSwordHitEnemy = true;
        }
    }
}

void Player::ManageAnimations() {
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
    mDrawAnimatedComponent->UseFlip(false);
    if (mIsDead) {
        mDrawAnimatedComponent->SetAnimation("die");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mDeathAnimationDuration);
    }
    else if (mHurtTimer < mHurtDuration) {
        mDrawAnimatedComponent->SetAnimation("hurt");
    }
    else if (mDashComponent->GetIsDashing()) {
        mDrawAnimatedComponent->SetAnimation("dash");
    }
    else if (mSword->GetState() == ActorState::Active) {
        if (mSword->GetRotation() == 3 * Math::Pi / 2) {
            mDrawAnimatedComponent->SetAnimation("attackUp");
        }
        if (mSword->GetRotation() == Math::Pi / 2) {
            mDrawAnimatedComponent->SetAnimation("attackDown");
        }
        if (mSword->GetRotation() == 0 || mSword->GetRotation() == Math::Pi) {
            SetRotation(mSword->GetRotation());
            mDrawAnimatedComponent->SetAnimation("attackFront");
        }
        mDrawAnimatedComponent->SetAnimFPS(3.0f / 0.15f);
    }
    else if (mIsWallSliding && !mIsOnGround && mRigidBodyComponent->GetVelocity().y > 0) {
        mDrawAnimatedComponent->UseFlip(true);
        if (mWallSlideSide == WallSlideSide::left) {
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_NONE);
        }
        else if (mWallSlideSide == WallSlideSide::right) {
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_HORIZONTAL);
        }
        mDrawAnimatedComponent->SetAnimation("wallSlide");
    }
    else if (mIsRunning && mIsOnGround) {
        mDrawAnimatedComponent->SetAnimation("run");
    }
    else if (!mIsOnGround) {
        if (mRigidBodyComponent->GetVelocity().y < -200 * mGame->GetScale()) {
            mDrawAnimatedComponent->SetAnimation("jumpUp");
        }
        if (mRigidBodyComponent->GetVelocity().y > 200 * mGame->GetScale()) {
            mDrawAnimatedComponent->SetAnimation("falling");
        }
        if (mRigidBodyComponent->GetVelocity().y > -200 * mGame->GetScale() &&
            mRigidBodyComponent->GetVelocity().y < 200 * mGame->GetScale())
        {
            mDrawAnimatedComponent->SetAnimation("jumpApex");
        }
    }
    else if (mIsHealing) {
        mDrawAnimatedComponent->SetAnimation("heal");
        mDrawAnimatedComponent->SetAnimFPS(10.0f / (mHealAnimationDuration));
    }
    else {
        mDrawAnimatedComponent->SetAnimation("idle");
    }

    if (mIsInvulnerable && mHurtTimer > mHurtDuration) {
        if (mBlink) {
            mDrawAnimatedComponent->SetTransparency(100);
        }
        else {
            mDrawAnimatedComponent->SetTransparency(255);
        }
    }
    else {
        mDrawAnimatedComponent->SetTransparency(255);
    }
}


void Player::ReceiveHit(float damage, Vector2 knockBackDirection) {
    if (!mIsInvulnerable && mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        mHealthPoints -= damage;
        mIsInvulnerable = true;
        mHurtTimer = 0;
        mDrawAnimatedComponent->ResetAnimationTimer();

        Vector2 vel = mRigidBodyComponent->GetVelocity();
        if (vel.Length() > 0) {
            vel.Normalize();
        }

        // ParticleSystem* blood = new ParticleSystem(mGame, 10, 170.0, 3.0, 0.07f);
        // blood->SetEmitDirection(knockBackDirection);
        // blood->SetPosition(GetPosition());
        // blood->SetParticleColor(SDL_Color{170, 113, 84, 255});

        // Uint8 color1 = 10;
        // Uint8 color2 = 200;
        // SDL_Color color{color2, color2, color2, 255};

        // for (int i = 0; i < 3; i++) {
        //     if (color.r == color1) {
        //         color = {color2, color2, color2, 255};
        //     }
        //     else {
        //         color = {color1, color1, color1, 255};
        //     }
        //     auto* effect = new Effect(mGame);
        //     effect->SetDuration(0.3f);
        //     effect->SetPosition(GetPosition());
        //     effect->SetSize(320);
        //     effect->SetColor(color);
        //     effect->SetEffect(TargetEffect::swordHit);
        // }

        Vector2 knockBack = knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3);
        knockBack.Normalize();
        mRigidBodyComponent->SetVelocity(knockBack * mKnockBackSpeed);
        // mRigidBodyComponent->SetVelocity(knockBackDirection * mKnockBackSpeed + vel * (mKnockBackSpeed / 3));
        mKnockBackTimer = 0;
        mInvulnerableTimer = 0;
        mGame->ActiveHitStop();
        mHealAnimationTimer = 0;
        mGame->GetCamera()->StartCameraShake(0.5, mCameraShakeStrength);
        mGame->GetAudio()->PlaySound("Damage/Damage.wav");
    }
}

bool Player::Died() {
    if (mHealthPoints <= 0) {
        if (mIsDead == false) {
            mIsDead = true;
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        return true;
    }
    return false;
}

void Player::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    SetStartingPosition(Vector2(mStartingPosition.x / oldScale * newScale, mStartingPosition.y / oldScale * newScale));
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    mJumpForce = mJumpForce / oldScale * newScale;
    mLowGravity = mLowGravity / oldScale * newScale;
    mMediumGravity = mMediumGravity / oldScale * newScale;
    mHighGravity = mHighGravity / oldScale * newScale;
    mSwordWidth = mSwordWidth / oldScale * newScale;
    mSwordHeight = mSwordHeight / oldScale * newScale;
    mFireballRecoil = mFireballRecoil / oldScale * newScale;
    mFireballWidth = mFireballWidth / oldScale * newScale;
    mFireBallHeight = mFireBallHeight / oldScale * newScale;
    mFireballSpeed = mFireballSpeed / oldScale * newScale;
    mWallSlideSpeed = mWallSlideSpeed / oldScale * newScale;
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mHookSpeed = mHookSpeed / oldScale * newScale;
    mHookAmplitude = mHookAmplitude / oldScale * newScale;
    mHookSegmentHeight = mHookSegmentHeight / oldScale * newScale;

    mHookEnd.x = mHookEnd.x / oldScale * newScale;
    mHookEnd.y = mHookEnd.y / oldScale * newScale;

    if (mDrawRopeComponent) {
        mDrawRopeComponent->SetNumSegments(mHookSegments);
        mDrawRopeComponent->SetAmplitude(mHookAmplitude);
        mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);
    }

    mRigidBodyComponent->SetMaxSpeedX(mRigidBodyComponent->GetMaxSpeedX() / oldScale * newScale);
    mRigidBodyComponent->SetMaxSpeedY(mRigidBodyComponent->GetMaxSpeedY() / oldScale * newScale);
    mDashComponent->SetDashSpeed(mDashComponent->GetDashSpeed() / oldScale * newScale);

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth * 3.6f);
    mDrawAnimatedComponent->SetHeight(mWidth * 3.6f);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
