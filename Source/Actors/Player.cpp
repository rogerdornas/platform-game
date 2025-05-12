//
// Created by roger on 22/04/2025.
//

#include "Player.h"

#include "../Game.h"
#include "../Actors/Sword.h"
#include "../Actors/FireBall.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DashComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"

Player::Player(Game *game, float width, float height)
    :Actor(game)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

    ,mStartingPosition(Vector2::Zero)

    ,mWidth(width)
    ,mHeight(height)

    ,mIsOnGround(false)
    ,mIsOnMovingGround(false)
    ,mMovingGroundVelocity(Vector2::Zero)

    ,mIsJumping(false)
    ,mCanJump(true)
    ,mMaxJumpTime(0.25f)
    ,mJumpTimer(0.0f)
    ,mJumpForce(-1000.0f)
    ,mMoveSpeed(700)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)

    ,mCanDash(true)

    ,mPrevSwordPressed(false)
    ,mSwordCooldownDuration(0.4f)
    ,mSwordCooldownTimer(0.0f)
    ,mSwordDirection(0)
    ,mSwordHitedEnemy(false)

    ,mCanFireBall(true)
    ,mPrevFireBallPressed(false)
    ,mFireBallCooldownDuration(1.0f)
    ,mFireBallCooldownTimer(0.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mStopInAirFireBallTimer(0.0f)
    ,mFireballRecoil(0.0f)

    ,mCanWallSlide(true)
    ,mIsWallSliding(false)
    ,mWallSlideSide(WallSlideSide::notSliding)
    ,mWallSlideSpeed(300)
    ,mTryingLeavingWallSlideLeft(0)
    ,mTryingLeavingWallSlideRight(0)
    ,mTimerToLeaveWallSlidingLeft(0.15f)
    ,mTimerToLeaveWallSlidingRight(0.15f)
    ,mMaxTimerToLiveWallSliding(0.15f)

    ,mWallJumpTimer(0.15f)
    ,mWallJumpMaxTime(0.15f)

    ,mKnockBackSpeed(800.0f)
    ,mKnockBackDuration(0.2f)
    ,mKnockBackTimer(0.0f)

    ,mIsRunning(false)
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

    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Esquilo/zenzen.png", 100, 100, 1000);
    // mDrawAnimatedComponent = new DrawAnimatedComponent(this, 100, 100, "../Assets/Sprites/Raposa/Raposa.png", "../Assets/Sprites/Raposa/Raposa.json", 1000);

    // std::vector<int> idle = {0};
    // mDrawAnimatedComponent->AddAnimation("idle", idle);

    // std::vector<int> run = {1, 2, 3, 4, 5};
    // mDrawAnimatedComponent->AddAnimation("run", run);

    // mDrawAnimatedComponent->SetAnimation("idle");
    // mDrawAnimatedComponent->SetAnimFPS(16.0f);


    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {255, 255, 0, 255});
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1600);
    mAABBComponent = new AABBComponent(this, v1, v3);
    mDashComponent = new DashComponent(this, 1500, 0.2f, 0.5f);

    mSword = new Sword(game, 180, 113, 0.2f, 10.0f);

}

void Player::OnProcessInput(const uint8_t* state, SDL_GameController& controller) {
    mTryingLeavingWallSlideLeft = 0;
    mTryingLeavingWallSlideRight = 0;
    mIsRunning = false;
    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetAnimFPS(13.0f);
    }

    bool left = (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_LCTRL]) || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -20000;
    bool leftSlow = (state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_LCTRL]) || (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -10000 && SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > -20000);
    bool right = (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LCTRL]) || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 20000;
    bool rightSlow = (state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LCTRL]) || (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 10000 && SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < 20000);
    bool lookUp = state[SDL_SCANCODE_UP] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_UP) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) < -28000;
    bool lodDown = state[SDL_SCANCODE_DOWN] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) > 28000;
    bool up = state[SDL_SCANCODE_UP] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_UP) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) < -28000;
    bool down = state[SDL_SCANCODE_DOWN] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) || SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) > 28000;
    bool jump = state[SDL_SCANCODE_Z] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_A);
    bool dash = state[SDL_SCANCODE_C] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    bool sword = state[SDL_SCANCODE_X] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_X);
    bool fireBall = state[SDL_SCANCODE_A] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_B);

    if (!left && !leftSlow && !right && !rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration)) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    else {
        if (left && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (leftSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration)) {
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideLeft = 1;
                if (mTimerToLeaveWallSlidingLeft >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1 + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideLeft = 0;
                    mTimerToLeaveWallSlidingLeft = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1 + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingLeft = 0;
            }
        }

        if (right && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }

        if (rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking && (mWallJumpTimer >= mWallJumpMaxTime) && (mKnockBackTimer >= mKnockBackDuration)) {
            SetRotation(0);
            mSwordDirection = 0;
            if (mIsWallSliding && !mIsOnGround) {
                mTryingLeavingWallSlideRight = 1;
                if (mTimerToLeaveWallSlidingRight >= mMaxTimerToLiveWallSliding) {
                    mIsRunning = true;
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1 + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                    mTryingLeavingWallSlideRight = 0;
                    mTimerToLeaveWallSlidingRight = 0;
                }
            }
            else {
                mIsRunning = true;
                mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1 + mMovingGroundVelocity.x, mRigidBodyComponent->GetVelocity().y));
                mTimerToLeaveWallSlidingRight = 0;
            }
        }
    }

    if (lookUp) {
        GetGame()->GetCamera()->mLookUp = true;
    }
    if (lodDown) {
        GetGame()->GetCamera()->mLookDown = true;
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
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce) + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
            }
            // Wall jumping
            if (mIsWallSliding && !mIsJumping && mCanJump) {
                if (mWallSlideSide == WallSlideSide::left) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                }
                else {
                    mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mJumpForce) + mMovingGroundVelocity);
                }
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mWallJumpTimer = 0;
            }
            // Pulo no ar
            if (!(mIsOnGround || mIsWallSliding) && mJumpCountInAir < mMaxJumpsInAir && mCanJump && (mWallJumpTimer >= mWallJumpMaxTime)) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce) + mMovingGroundVelocity);
                mIsJumping = true;
                mCanJump = false;
                mJumpTimer = 0.0f;
                mJumpCountInAir++; // Incrementa número de pulos
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
        }
    }

    // Sword
    // Detecta borda de descida da tecla K e cooldown pronto
    if (sword && !mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration) {
        // Ativa a espada
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(mSwordDirection);
        mSword->SetPosition(GetPosition());
        mSwordHitedEnemy = false;

        // Inicia cooldown
        mSwordCooldownTimer = 0;
    }
    mPrevSwordPressed = sword;

    // FireBall
    if (mCanFireBall) {
        if (fireBall && !mPrevFireBallPressed && mFireBallCooldownTimer >= mFireBallCooldownDuration) {
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
        mPrevFireBallPressed = fireBall;
    }
}

void Player::OnUpdate(float deltaTime)
{
    if (mSwordCooldownTimer < mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
    }

    if (mKnockBackTimer < mKnockBackDuration) {
        mKnockBackTimer += deltaTime;
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
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 100 * deltaTime));
            }
        } else {
            mIsJumping = false;
            // Gravidade
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
            }
        }
    }
    else {
        // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
        }
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();


    // Se cair, volta para a posição inicial
    if (GetPosition().y > 20000) {
        SetPosition(mStartingPosition);
    }
    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }

}

void Player::ResolveGroundCollision() {
    std::array<bool, 4> collisionSide;
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colosão com ground
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
                    mIsOnGround = true;
                    mIsJumping  = false;
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
                            SetPosition(Vector2(GetPosition().x, g->GetPosition().y - g->GetHeight() / 2 - mHeight / 2 + 1)); // Gambiarra (Ao detectar colisão por cima em moving ground, desce o player 1 pixel para baixo para não ficar trepidando
                        }
                    }
                }

                // colidiu bot
                if (collisionSide[1]) {
                    mJumpTimer = mMaxJumpTime;
                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, 1));
                    if (g->GetIsMoving()) {
                        if (g->GetComponent<RigidBodyComponent>()->GetVelocity().y > 0) {
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, g->GetComponent<RigidBodyComponent>()->GetVelocity().y * 1.5)); // Para não grudar quando pular por baixo de uma plataforma movel
                        }
                    }
                }

                //colidiu pelas laterias
                if (mCanWallSlide) {
                    if ((collisionSide[2] || collisionSide[3])) {
                        // Move o player junto ao ground em movimento
                        if (g->GetIsMoving()) {
                            mIsOnMovingGround = true;
                            mMovingGroundVelocity = g->GetComponent<RigidBodyComponent>()->GetVelocity();
                        }

                        mIsWallSliding = true;
                        if (collisionSide[2] && !mDashComponent->GetIsDashing()) { // Testa se não está dashando para não bugar quando dar um dash na quina de baixo e inverter a direção do dash
                            mWallSlideSide = WallSlideSide::left;
                            if (!mIsOnGround) {
                                // SetRotation(Math::Pi);
                            }
                            if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x + 100, mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + 100, mRigidBodyComponent->GetVelocity().y));
                            }
                        }
                        else if (collisionSide[3] && !mDashComponent->GetIsDashing()) {
                            mWallSlideSide = WallSlideSide::right;
                            if (!mIsOnGround) {
                                // SetRotation(0);
                            }
                            if (mMovingGroundVelocity.x < 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x - 100, mRigidBodyComponent->GetVelocity().y));
                            }
                            else if (mMovingGroundVelocity.x > 0) {
                                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x - 100, mRigidBodyComponent->GetVelocity().y));
                            }
                        }
                        // Resetar dash no ar
                        mDashComponent->SetHasDashedInAir(false);
                        // RESET DO CONTADOR DE PULO
                        mJumpCountInAir = 0;
                        if (mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y - mMovingGroundVelocity.y > 0) {
                            mIsJumping  = false;
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x + mMovingGroundVelocity.x, mWallSlideSpeed + mMovingGroundVelocity.y));
                        }
                        else if (!mIsOnMovingGround && mRigidBodyComponent->GetVelocity().y > 0) {
                            mIsJumping  = false;
                            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mWallSlideSpeed));
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    // SetPosition(mStartingPosition);

                    Vector2 vel = mRigidBodyComponent->GetVelocity();
                    collisionSide = mAABBComponent->ResolveColision(*g->GetComponent<AABBComponent>());

                    mDashComponent->StopDash();

                    // Colidiu top
                    if (collisionSide[0]) {
                        mRigidBodyComponent->SetVelocity(Vector2(vel.x, -mKnockBackSpeed));
                    }
                    // Colidiu bot
                    if (collisionSide[1]) {
                        mRigidBodyComponent->SetVelocity(Vector2(vel.x, mKnockBackSpeed));
                    }
                    //Colidiu left
                    if (collisionSide[2]) {
                        mRigidBodyComponent->SetVelocity(Vector2(-mKnockBackSpeed, vel.y));
                    }
                    //Colidiu right
                    if (collisionSide[3]) {
                        mRigidBodyComponent->SetVelocity(Vector2(mKnockBackSpeed, vel.y));
                    }

                    mKnockBackTimer = 0;
                }
                else if (mSword->GetComponent<AABBComponent>()->Intersect(*g->GetComponent<AABBComponent>())) { // Colisão da sword com spikes
                    if (mSword->GetRotation() == Math::Pi / 2) {
                        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
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
    std::array<bool, 4> collisionSide;
    std::vector<Enemy*> enemys;
    enemys = GetGame()->GetEnemys();
    if (!enemys.empty()) {
        for (Enemy* e : enemys) {
            if (mAABBComponent->Intersect(*e->GetComponent<AABBComponent>())) {
                // SetPosition(mStartingPosition);

                Vector2 vel = mRigidBodyComponent->GetVelocity();
                collisionSide = mAABBComponent->ResolveColision(*e->GetComponent<AABBComponent>());

                mDashComponent->StopDash();

                // Colidiu top
                if (collisionSide[0]) {
                    mRigidBodyComponent->SetVelocity(Vector2(vel.x, -mKnockBackSpeed));
                }
                // Colidiu bot
                if (collisionSide[1]) {
                    mRigidBodyComponent->SetVelocity(Vector2(vel.x, mKnockBackSpeed));
                }
                //Colidiu left
                if (collisionSide[2]) {
                    mRigidBodyComponent->SetVelocity(Vector2(-mKnockBackSpeed, vel.y));
                }
                //Colidiu right
                if (collisionSide[3]) {
                    mRigidBodyComponent->SetVelocity(Vector2(mKnockBackSpeed, vel.y));
                }

                mKnockBackTimer = 0;
            }
            else if (mSword->GetComponent<AABBComponent>()->Intersect(*e->GetComponent<AABBComponent>())) { // Colisão da sword com enemys
                if (!mSwordHitedEnemy) {
                    e->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
                    mSwordHitedEnemy = true;
                }
                if (mSword->GetRotation() == Math::Pi / 2) {
                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce));
                    // Resetar dash no ar
                    mDashComponent->SetHasDashedInAir(false);
                    // RESET DO CONTADOR DE PULO
                    mJumpCountInAir = 0;
                }
            }
        }
    }
}


void Player::ManageAnimations() {
    if (mIsRunning && mIsOnGround) {
        mDrawAnimatedComponent->SetAnimation("run");
    }
    else {
        mDrawAnimatedComponent->SetAnimation("idle");
    }
}
