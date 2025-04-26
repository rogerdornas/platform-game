//
// Created by roger on 22/04/2025.
//

#include "Player.h"
#include "../Game.h"
#include "../Actors/Sword.h"
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
    ,mCanJump(true) // Evita segurar botao de pular para continuar pulando
    ,mJumpTime(0.0f)
    ,mMaxJumpTime(0.25f)
    ,mJumpForce(-850.0f)
    ,mMoveSpeed(550)
    ,mJumpCountInAir(0)
    ,mMaxJumpsInAir(1)
    ,mPrevSwordPressed(false)
    ,mSwordCooldownTimer(0.0f)
    ,mSwordCooldownDuration(0.3f)
    ,mSwordDirection(0)
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
    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3, {255, 255, 0, 255});
    mDashComponent = new DashComponent(this, 1500, 0.18f, 0.5f);

    mSword = new Sword(game, 60, 20, 0.1f);
}

void Player::OnProcessInput(const uint8_t* state) {
    if (!(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT]) && !mDashComponent->GetIsDashing()) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    else {
        if (state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing()) {
            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
        }

        if (state[SDL_SCANCODE_LEFT] && state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing()) {
            mRigidBodyComponent->SetVelocity(Vector2(-mMoveSpeed/20, mRigidBodyComponent->GetVelocity().y));
            SetRotation(Math::Pi);
            mSwordDirection = Math::Pi;
        }

        if (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing()) {
            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed, mRigidBodyComponent->GetVelocity().y));
            SetRotation(0);
            mSwordDirection = 0;
        }

        if (state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LCTRL] && !mDashComponent->GetIsDashing()) {
            mRigidBodyComponent->SetVelocity(Vector2(mMoveSpeed/20, mRigidBodyComponent->GetVelocity().y));
            SetRotation(0);
            mSwordDirection = 0;
        }
    }

    if (!(state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_UP])) {
        mSwordDirection = GetRotation();
    }
    else {
        if (state[SDL_SCANCODE_DOWN]) {
            mSwordDirection = Math::Pi / 2;
        }
        if (state[SDL_SCANCODE_UP]) {
            mSwordDirection = 3 * Math::Pi / 2;
        }
    }

    //Início do pulo
    if (state[SDL_SCANCODE_Z]) {
        if (!mDashComponent->GetIsDashing()) {
            if (mIsOnGround && !mIsJumping && mCanJump) {
                mRigidBodyComponent->SetVelocity(Vector2(0, mJumpForce));
                mIsJumping = true;
                mCanJump = false;
                mJumpTime = 0.0f;
            }
            if (!mIsOnGround && mJumpCountInAir < mMaxJumpsInAir && mCanJump) {
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
    if (state[SDL_SCANCODE_C]) {
        mDashComponent->UseDash(mIsOnGround);
    }

    // Sword
    // Detecta borda de descida da tecla K e cooldown pronto
    if (state[SDL_SCANCODE_X] && !mPrevSwordPressed && mSwordCooldownTimer >= mSwordCooldownDuration) {
        // Ativa a espada
        mSword->SetState(ActorState::Active);
        mSword->SetRotation(mSwordDirection);
        mSword->SetPosition(GetPosition() + mSword->GetForward() * (mWidth / 2) * 3);

        // Inicia cooldown
        mSwordCooldownTimer = 0;
    }

    mPrevSwordPressed = state[SDL_SCANCODE_X];

}

void Player::OnUpdate(float deltaTime)
{
    if (mSwordCooldownTimer <= mSwordCooldownDuration) {
        mSwordCooldownTimer += deltaTime;
    }
    mIsOnGround = false;

    if (mIsJumping) {
        mJumpTime += deltaTime;
        if (mJumpTime <= mMaxJumpTime) {
            // Gravidade menor
            // So aplica gravidade se nao estiver dashando
            if (!mDashComponent->GetIsDashing()) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 100 * deltaTime));
            }
        } else {
            mIsJumping = false;
            // Gravidade
            // So aplica gravidade se nao estiver dashando
            if (!mDashComponent->GetIsDashing()) {
                mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
            }
        }
    }
    else {
        // So aplica gravidade se nao estiver dashando
        if (!mDashComponent->GetIsDashing()) {
            mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, mRigidBodyComponent->GetVelocity().y + 3000 * deltaTime));
        }
    }

    // Colisao com ground
    std::vector<Ground*> grounds;
    grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* c : grounds) {
            if (mAABBComponent->Intersect(*c->GetComponent<AABBComponent>())) {
                std::array<bool, 4> collisionSide = mAABBComponent->ResolveColision(*c->GetComponent<AABBComponent>());
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
                if (collisionSide[1]) {
                    mJumpTime = mMaxJumpTime;
                    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x, 1));
                }
            }
        }
    }
}
