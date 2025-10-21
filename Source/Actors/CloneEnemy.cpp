//
// Created by roger on 09/10/2025.
//

#include "CloneEnemy.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DashComponent.h"

CloneEnemy::CloneEnemy(Game *game)
    :Enemy(game)
    ,mIsOnGround(false)
    ,mIsOnMovingGround(false)
    ,mMovingGroundVelocity(Vector2::Zero)

    ,mMaxTimeOutOfGroundToJump(0.07)
    ,mTimerOutOfGroundToJump(0.0f)
    ,mMaxTimeOutOfWallToJump(0.07)
    ,mTimerOutOfWallToJump(0.0f)

    ,mIsJumping(false)
    ,mJumpTimer(0.0f)
    ,mMaxJumpTime(0.33f)
    ,mJumpForce(-750.0f * mGame->GetScale())
    ,mCanJump(true)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)
    ,mLowGravity(50.0f * mGame->GetScale())
    ,mMediumGravity(3300.0f * mGame->GetScale())
    ,mHighGravity(4500.0f * mGame->GetScale())

    ,mCanDash(true)

    ,mPrevSwordPressed(false)
    ,mSwordCooldownTimer(0.0f)
    ,mSwordCooldownDuration(0.4f)
    ,mSwordDamage(10.0f)
    ,mSwordDirection(0)
    ,mSwordHitEnemy(false)
    ,mSwordHitGround(false)
    ,mSwordHitSpike(false)
    ,mSwordHitPlayer(false)

    ,mCanFireBall(true)
    ,mPrevFireBallPressed(false)
    ,mFireBallCooldownDuration(0.1f)
    ,mFireBallCooldownTimer(0.0f)
    ,mIsFireAttacking(false)
    ,mStopInAirFireBallTimer(0.0f)
    ,mStopInAirFireBallMaxDuration(0.0f)
    ,mFireballRecoil(0.0f * mGame->GetScale())
    ,mFireballWidth(45 * mGame->GetScale())
    ,mFireBallHeight(45 * mGame->GetScale())
    ,mFireballSpeed(1500 * mGame->GetScale())
    ,mFireballDamage(10.0f)
    ,mMaxMana(90.0f)
    ,mMana(90.0f)
    ,mManaIncreaseRate(6.0f)
    ,mFireballManaCost(30.0f)
    ,mFireballAnimationDuration(0.2f)
    ,mFireballAnimationTimer(mFireballAnimationDuration)

    ,mHealAmount(5.0f)
    ,mIsHealing(false)
    ,mHealAnimationDuration(0.8f)
    ,mHealAnimationTimer(0.0f)

    ,mIsRunning(false)
    ,mStopRunningDuration(0.05f)
    ,mStopRunningTimer(0.0f)
    ,mHurtDuration(0.2f)
    ,mBlink(false)
    ,mBlinkDuration(0.03f)
    ,mBlinkTimer(mBlinkDuration)

    ,mRunningSoundIntervalDuration(0.3f)
    ,mRunningSoundIntervalTimer(0.0f)
    ,mWasOnGround(false)
    ,mDashComponent(nullptr)
{
    mWidth = 45 * mGame->GetScale();
    mHeight = 75 * mGame->GetScale();
    mMoveSpeed = 700 * mGame->GetScale();
    mSwordWidth = mWidth * 3.0f;
    mSwordHeight = mHeight * 1.3f;
    mFlashTimer = mHurtDuration;
    mHealthPoints = 40;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 15;
    mMoneyDrop = 0;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.1f;
    mKnockBackTimer = mKnockBackDuration;

    SetSize(mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 4.93f, mWidth * 4.93f * 1.11f,
                                                   "../Assets/Sprites/Esquilo5/Esquilo.png",
                                                   "../Assets/Sprites/Esquilo5/Esquilo.json", 1002);

    std::vector idle = {21, 22, 23, 24};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector attackFront = {21, 2, 3};
    mDrawAnimatedComponent->AddAnimation("attackFront", attackFront);

    std::vector attackUp = {21, 4, 5};
    mDrawAnimatedComponent->AddAnimation("attackUp", attackUp);

    std::vector attackDown = {21, 0, 1};
    mDrawAnimatedComponent->AddAnimation("attackDown", attackDown);

    std::vector fireball = {12, 13};
    mDrawAnimatedComponent->AddAnimation("fireball", fireball);

    std::vector dash = {6, 7, 7, 7, 8};
    mDrawAnimatedComponent->AddAnimation("dash", dash);

    std::vector run = {28, 29, 30, 31, 32, 33};
    mDrawAnimatedComponent->AddAnimation("run", run);

    std::vector heal = {14, 15, 16, 17, 18, 18, 17, 16, 15, 14};
    mDrawAnimatedComponent->AddAnimation("heal", heal);

    std::vector wallSlide = {34};
    mDrawAnimatedComponent->AddAnimation("wallSlide", wallSlide);

    std::vector hurt = {19, 20};
    mDrawAnimatedComponent->AddAnimation("hurt", hurt);

    std::vector die = {19, 9, 10, 11, 11, 11};
    mDrawAnimatedComponent->AddAnimation("die", die);

    std::vector jumpUp = {25};
    mDrawAnimatedComponent->AddAnimation("jumpUp", jumpUp);

    std::vector jumpApex = {26};
    mDrawAnimatedComponent->AddAnimation("jumpApex", jumpApex);

    std::vector falling = {27};
    mDrawAnimatedComponent->AddAnimation("falling", falling);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
    mDrawAnimatedComponent->SetTransparency(170);

    mDashComponent = new DashComponent(this, 1500 * mGame->GetScale(), 0.2f, 0.5f);

    mSword = new Sword(mGame, this, mSwordWidth, mSwordHeight, 0.15f, mSwordDamage);

    // Pool de Jump Effects
    // for (int i = 0; i < 5; i++) {
    //     auto* jumpEffect = new JumpEffect(mGame, this, 0.3f);
    //     mJumpEffects.emplace_back(jumpEffect);
    // }
}

void CloneEnemy::OnProcessInput(const uint8_t* state, SDL_GameController &controller) {
    mIsRunning = false;

    if (!IsOnScreen()) {
        return;
    }

    InputCommand inputCommand;

    inputCommand.right = (mGame->IsActionPressed(Game::Action::MoveLeft, state, &controller) &&
                !mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -20000;


    inputCommand.rightSlow = (mGame->IsActionPressed(Game::Action::MoveLeft, state, &controller) &&
                    mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                    (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < -10000 &&
                    SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > -20000);


    inputCommand.left = (mGame->IsActionPressed(Game::Action::MoveRight, state, &controller) &&
                 !mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                 SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 20000;


    inputCommand.leftSlow = (mGame->IsActionPressed(Game::Action::MoveRight, state, &controller) &&
                     mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                     (SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) > 10000 &&
                     SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTX) < 20000);


    inputCommand.lookUp = (!inputCommand.right && !inputCommand.rightSlow && !inputCommand.left && !inputCommand.leftSlow && mIsOnGround) &&
                  ((mGame->IsActionPressed(Game::Action::Up, state, &controller) &&
                  mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                  SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) < -28000);


    inputCommand.lookDown = (!inputCommand.right && !inputCommand.rightSlow && !inputCommand.left && !inputCommand.leftSlow && mIsOnGround) &&
                   ((mGame->IsActionPressed(Game::Action::Down, state, &controller) &&
                   mGame->IsActionPressed(Game::Action::Look, state, &controller)) ||
                   SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_RIGHTY) > 28000);


    inputCommand.up = mGame->IsActionPressed(Game::Action::Up, state, &controller) ||
              SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) < -22000;

    inputCommand.down = mGame->IsActionPressed(Game::Action::Down, state, &controller) ||
          SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_LEFTY) > 22000;


    inputCommand.jump = mGame->IsActionPressed(Game::Action::Jump, state, &controller);


    inputCommand.dash = mGame->IsActionPressed(Game::Action::Dash, state, &controller);


    inputCommand.sword = mGame->IsActionPressed(Game::Action::Attack, state, &controller);

    inputCommand.fireBall = mGame->IsActionPressed(Game::Action::FireBall, state, &controller);


    inputCommand.heal = mGame->IsActionPressed(Game::Action::Heal, state, &controller) ||
                SDL_GameControllerGetAxis(&controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 10000;

    mInputCommands.emplace_back(inputCommand);

    for (auto it = mInputCommands.begin(); it != mInputCommands.end();) {
        if (it->delay <= 0) {
            if (!it->left && !it->leftSlow && !it->right && !it->rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
                !mIsOnMovingGround && (mKnockBackTimer >= mKnockBackDuration)) {
                if (mKnockBackTimer >= mKnockBackDuration) {
                    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
                }
            }
            else {
                if (it->left && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
                    (mKnockBackTimer >= mKnockBackDuration)) {
                    SetRotation(Math::Pi);
                    mSwordDirection = Math::Pi;
                    if (!mIsOnGround) {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                    else {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                }

                if (it->leftSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
                    (mKnockBackTimer >= mKnockBackDuration)) {
                    SetRotation(Math::Pi);
                    mSwordDirection = Math::Pi;
                    if (!mIsOnGround) {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                    else {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                }

                if (it->right && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
                    (mKnockBackTimer >= mKnockBackDuration)) {
                    SetRotation(0);
                    mSwordDirection = 0;
                    if (!mIsOnGround) {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                    else {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                }

                if (it->rightSlow && !mDashComponent->GetIsDashing() && !mIsFireAttacking &&
                    (mKnockBackTimer >= mKnockBackDuration)) {
                    SetRotation(0);
                    mSwordDirection = 0;
                    if (!mIsOnGround) {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                    else {
                        mIsRunning = true;
                        if (mKnockBackTimer >= mKnockBackDuration) {
                            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed * 0.1f + mMovingGroundVelocity.x,
                                                                     mRigidBodyComponent->GetVelocity().y));
                        }
                    }
                }
            }

            if (it->lookUp) {
                mGame->GetCamera()->SetLookUp();
            }

            if (it->lookDown) {
                mGame->GetCamera()->SetLookDown();
            }

            if (!it->down && !it->up) {
                mSwordDirection = GetRotation();
            }
            else {
                if (it->down) {
                    mSwordDirection = Math::Pi / 2;
                }
                if (it->up) {
                    mSwordDirection = 3 * Math::Pi / 2;
                }
            }

            // Dash
            if (mCanDash) {
                if (it->dash && !mIsFireAttacking) {
                    if (mDashComponent->UseDash(mIsOnGround) && mIsOnGround) {
                        for (JumpEffect* j: mJumpEffects) {
                            if (j->GetState() == ActorState::Paused) {
                                j->SetState(ActorState::Active);
                                j->StartEffect(JumpEffect::EffectType::TakeOff);
                                break;
                            }
                        }
                    }
                }
            }

            //Início do pulo
            if (it->jump && !mIsFireAttacking) {
                if (!mDashComponent->GetIsDashing()) {
                    // Pulo do chao
                    if ((mTimerOutOfGroundToJump < mMaxTimeOutOfGroundToJump) && !mIsJumping && mCanJump) {
                        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce)
                                                         + mMovingGroundVelocity);
                        mIsJumping = true;
                        mCanJump = false;
                        mJumpTimer = 0.0f;
                        mTimerOutOfGroundToJump = mMaxTimeOutOfGroundToJump;
                        mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
                        for (JumpEffect* j: mJumpEffects) {
                            if (j->GetState() == ActorState::Paused) {
                                j->SetState(ActorState::Active);
                                j->StartEffect(JumpEffect::EffectType::TakeOff);
                                break;
                            }
                        }
                    }
                    // Pulo no ar
                    if (!(mIsOnGround) && mJumpCountInAir < mMaxJumpsInAir && mCanJump) {
                        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mJumpForce * 0.8f)
                                                         + mMovingGroundVelocity);
                        mIsJumping = true;
                        mCanJump = false;
                        mJumpTimer = mMaxJumpTime * 0.4f;
                        mJumpCountInAir++; // Incrementa número de pulos
                        mGame->GetAudio()->PlaySound("Jump/Jump1.wav");
                        for (JumpEffect* j: mJumpEffects) {
                            if (j->GetState() == ActorState::Paused) {
                                j->SetState(ActorState::Active);
                                j->StartEffect(JumpEffect::EffectType::DoubleJump);
                                break;
                            }
                        }
                    }
                }
            }
            else {
                mIsJumping = false; // jogador soltou o W
                mCanJump = true;
            }

            // Sword
            // Detecta borda de descida da tecla K e cooldown pronto
            if (it->sword && !mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration) {
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->ResetAnimationTimer();
                }
                mGame->GetAudio()->PlayVariantSound("SwordSlash/SwordSlash.wav", 11);
                // Ativa a espada
                mSwordHitPlayer = false;
                mSword->SetState(ActorState::Active);
                mSword->SetRotation(mSwordDirection);
                mSword->SetPosition(GetPosition());
                mSwordHitEnemy = false;
                mSwordHitGround = false;
                mSwordHitSpike = false;

                // Inicia cooldown
                mSwordCooldownTimer = 0;
            }
            mPrevSwordPressed = it->sword;

            // FireBall
            if (mCanFireBall) {
                if (it->fireBall &&
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
                            f->SetIsFromEnemy();
                            f->SetPosition(GetPosition() + f->GetForward() * (f->GetWidth() / 2));
                            mIsFireAttacking = true;
                            mStopInAirFireBallTimer = 0;
                            mFireballAnimationTimer = 0;
                            if (mDrawAnimatedComponent) {
                                mDrawAnimatedComponent->ResetAnimationTimer();
                            }
                            mMana -= mFireballManaCost;
                            break;
                        }
                    }
                    // Inicia cooldown
                    mFireBallCooldownTimer = 0;
                }
                mPrevFireBallPressed = it->fireBall;
            }

            // Heal
            if (it->heal && mHealthPoints < mMaxHealthPoints && mIsOnGround) {
                if (it->left || it->leftSlow || it->right || it->rightSlow || it->jump || it->dash || it->sword || it->fireBall) {
                    mHealAnimationTimer = 0;
                    mIsHealing = false;
                }
                else {
                    mIsHealing = true;
                    if (mHealAnimationTimer >= mHealAnimationDuration) {
                        mHealAnimationTimer = 0;
                        mHealthPoints += mHealAmount;
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
            mInputCommands.erase(it);
        }
        else {
            ++it;
        }
    }
}

void CloneEnemy::OnUpdate(float deltaTime) {
    for (auto it = mInputCommands.begin(); it != mInputCommands.end(); ++it) {
        it->delay -= deltaTime;
    }

    if (mIsRunning) {
        mStopRunningTimer = 0;
    }
    else {
        mStopRunningTimer += deltaTime;
    }

    if (mFlashTimer < mHurtDuration) {
        if (mFlashTimer == 0) {
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->ResetAnimationTimer();
            }
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    if (mSwordCooldownTimer < mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }

    if (mFireBallCooldownTimer < mFireBallCooldownDuration) {
        mFireBallCooldownTimer += deltaTime;
    }

    if (mFireballAnimationTimer < mFireballAnimationDuration) {
        mFireballAnimationTimer += deltaTime;
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

    if (mMana < mMaxMana) {
        mMana += mManaIncreaseRate * deltaTime;
        if (mMana > mMaxMana) {
            mMana = mMaxMana;
        }
    }

    if (mIsHealing) {
        if (mHealAnimationTimer == 0) {
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->ResetAnimationTimer();
            }
        }
        mHealAnimationTimer += deltaTime;
    }

    mIsOnGround = false;
    mIsOnMovingGround = false;
    mMovingGroundVelocity = Vector2::Zero;

    if (mIsFireAttacking) {
        mRigidBodyComponent->SetVelocity(Vector2(-GetForward().x * mFireballRecoil, 0) + mMovingGroundVelocity);
    }

    if (mIsJumping) {
        mJumpTimer += deltaTime;
        if (mJumpTimer <= mMaxJumpTime) {
            // Gravidade menor
            // So aplica gravidade se nao estiver dashando e nao estiver tacando fireball
            if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround) {
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
        if (!mDashComponent->GetIsDashing() && !mIsFireAttacking && !mIsOnMovingGround && !mIsOnGround) {
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

    ResolvePlayerCollision();
    ResolveEnemyCollision();
    ResolveGroundCollision();

    if (mIsOnGround) {
        mTimerOutOfGroundToJump = 0;
    }
    else {
        mTimerOutOfGroundToJump += deltaTime;
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
            for (JumpEffect* j: mJumpEffects) {
                if (j->GetState() == ActorState::Paused) {
                    j->SetState(ActorState::Active);
                    j->StartEffect(JumpEffect::EffectType::Land);
                    break;
                }
            }
            mRunningSoundIntervalTimer = 0;
        }
    }

    mWasOnGround = mTimerOutOfGroundToJump < mMaxTimeOutOfGroundToJump;

    if (Died()) {
    }

    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }
}

void CloneEnemy::ResolveGroundCollision() {
    bool isCollidingSides = false;
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground* > grounds = mGame->GetGrounds();
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
                                    color = {102, 114, 145, 255};
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
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());

                    mDashComponent->StopDash();

                    ReceiveHit(10, collisionNormal);
                }
                if (mSword->GetComponent<ColliderComponent>()->Intersect(*g->GetComponent<ColliderComponent>())) { // Colisão da sword com spikes
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

void CloneEnemy::ResolvePlayerCollision() {
    Player* player = GetGame()->GetPlayer();

    if (mSword->GetComponent<ColliderComponent>()->Intersect(*player->GetComponent<ColliderComponent>())) { // Colisão da sword da fox com o player
        if (!mSwordHitPlayer) {
            player->ReceiveHit(mSword->GetDamage(), mSword->GetForward());
            mSwordHitPlayer = true;
        }
    }
}


void CloneEnemy::ManageAnimations() {
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
    mDrawAnimatedComponent->UseFlip(false);

    if (mFlashTimer< mHurtDuration) {
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
    else if (mFireballAnimationTimer < mFireballAnimationDuration) {
        mDrawAnimatedComponent->SetAnimation("fireball");
        mDrawAnimatedComponent->SetAnimFPS(2.0f / mFireballAnimationDuration);
    }
    else if (mStopRunningTimer < mStopRunningDuration && mIsOnGround) {
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
        mDrawAnimatedComponent->SetAnimFPS(6.0f);
    }
}

void CloneEnemy::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
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

    mRigidBodyComponent->SetMaxSpeedX(mRigidBodyComponent->GetMaxSpeedX() / oldScale * newScale);
    mRigidBodyComponent->SetMaxSpeedY(mRigidBodyComponent->GetMaxSpeedY() / oldScale * newScale);
    mDashComponent->SetDashSpeed(mDashComponent->GetDashSpeed() / oldScale * newScale);

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 4.93f);
        mDrawAnimatedComponent->SetHeight(mWidth * 4.93f * 1.11f);
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

    if (auto* aabb = dynamic_cast<AABBComponent*>(mColliderComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
