//
// Created by roger on 15/09/2025.
//

#include "HookEnemy.h"
#include <cfloat>
#include "Effect.h"
#include "ParticleSystem.h"
#include "Skill.h"
#include "../Game.h"
#include "../HUD.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/DrawRopeComponent.h"
#include "../Random.h"

HookEnemy::HookEnemy(Game *game)
    :Enemy(game)
    ,mHookEnemyState(State::Stop)

    ,mGravity(3000 * mGame->GetScale())
    ,mIsOnGround(true)

    ,mStopDuration(1.2f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.07f)

    ,mHookStateDuration(0.6f)
    ,mHookStateTimer(0.0f)
    ,mIsHooking(false)
    ,mHookDirection(Vector2::Zero)
    ,mHookSpeed(1600.0f * mGame->GetScale())
    ,mHookingDuration(0.20f)
    ,mHookingTimer(0.0f)
    ,mHookEnd(Vector2::Zero)
    ,mHookAnimProgress(0.0f)
    ,mIsHookAnimating(false)
    ,mHookPoint(nullptr)
    ,mHookAnimationDuration(0.45f)
    ,mHookSegments(20)
    ,mHookAmplitude(12.0f * mGame->GetScale())
    ,mHookSegmentHeight(10.0f * mGame->GetScale())

    ,mIsHookThrowing(false)
    ,mCurrentRopeTip(Vector2::Zero)
    ,mRopeThrowSpeed(3000.0f)

    ,mForwardAttackDuration(1.5f)
    ,mForwardAttackTimer(0.0f)
    ,mForwardAttackDelayDuration(0.35f)
    ,mForwardAttackDelayTimer(0.0f)
    ,mLastForwardAttackDirection(1)

    ,mSpeedIncreaseRate(3.5f)

    ,mDrawRopeComponent(nullptr)
{
    mWidth = 220 * mGame->GetScale();
    mHeight = 100 * mGame->GetScale();
    mMoveSpeed = 500 * mGame->GetScale();
    mHealthPoints = 750;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 50;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;
    mFreezeMax = 1000;
    mFrozenDecayRate = mFreezeMax / 3.0f;

    SetSize(mWidth, mHeight);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/HookEnemy/HookEnemy.png",
                                                    "../Assets/Sprites/HookEnemy/HookEnemy.json",
                                                    mWidth * 1.35f, mWidth * 1.35f * 0.73f, 998);
    std::vector idle = {2, 3, 4, 5};
    mDrawComponent->AddAnimation("idle", idle);

    std::vector fly = {1};
    mDrawComponent->AddAnimation("fly", fly);

    std::vector jump = {6};
    mDrawComponent->AddAnimation("jump", jump);

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(7.0f);


    mDrawRopeComponent = new DrawRopeComponent(this, "../Assets/Sprites/Rope/Rope2.png");
    mDrawRopeComponent->SetNumSegments(mHookSegments);
    mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);

    RemoveComponent(mColliderComponent);
    delete mColliderComponent;
    mColliderComponent = nullptr;

    mColliderComponent = new OBBComponent(this, Vector2(mWidth / 2, mHeight / 2));

    SetRotation(3 * Math::Pi / 2);
    SetTransformRotation(3 * Math::Pi / 2);
}

void HookEnemy::OnUpdate(float deltaTime) {
    mIsOnGround = false;
    mKnockBackTimer += deltaTime;
    if (mFlashTimer < mHitDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();
    ManageFreezing(deltaTime);

    // Gravidade
    if (!mIsOnGround && mHookEnemyState != State::ForwardAttack) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                 mRigidBodyComponent->GetVelocity().y
                                                 + mGravity * deltaTime));
    }

    if (!mIsFrozen) {
        if (mPlayerSpotted) {
            mGame->GetHUD()->StartBossFight(this);
            if (!mGame->GetBossMusicHandle().IsValid()) {
                mGame->StartBossMusic(mGame->GetAudio()->PlaySound("MantisLords.wav", true));
            }
            MovementAfterPlayerSpotted(deltaTime);
        }
        else {
            MovementBeforePlayerSpotted();
        }
    }

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    if (!mIsFrozen) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }

    if (mHealthPoints <= mMaxHealthPoints / 2) {
        mStopDuration = 1.0f;
    }

    // if (mDrawPolygonComponent) {
    //     if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
    //         auto verts = obb->GetVertices();
    //         mDrawPolygonComponent->SetVertices(verts);
    //     }
    // }
}

void HookEnemy::ResolveGroundCollision() {
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
                // colidiu laterais
                if (collisionNormal == Vector2::NegUnitX || collisionNormal == Vector2::UnitX) {
                    if (mHookEnemyState == State::DiagonalAttack) {
                        Player* player = GetGame()->GetPlayer();
                        float dist = GetPosition().x - player->GetPosition().x;
                        if (dist < 0) {
                            SetRotation(0.0);
                            SetTransformRotation(0.0f);
                            SetScale(Vector2(1,1));
                        }
                        else {
                            SetRotation(Math::Pi);
                            SetTransformRotation(0.0f);
                            SetScale(Vector2(-1,1));
                        }
                        mHookEnemyState = State::ForwardAttack;
                    }
                }
                if (collisionNormal == Vector2::NegUnitX && GetForward().x == 1) {
                    if (mHookEnemyState == State::ForwardAttack) {
                        mForwardAttackTimer = mForwardAttackDuration;
                        // SetPosition(GetPosition() + Vector2(-mHeight / 2, 0));
                    }
                }
                if (collisionNormal == Vector2::UnitX && GetForward().x == -1) {
                    if (mHookEnemyState == State::ForwardAttack) {
                        mForwardAttackTimer = mForwardAttackDuration;
                        // SetPosition(GetPosition() + Vector2(mHeight / 2, 0));
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

void HookEnemy::ResolveEnemyCollision() {
    std::vector<Enemy* > enemies = mGame->GetEnemies();
    if (!enemies.empty()) {
        for (Enemy* e: enemies) {
            if (e != this) {
                if (auto* hookEnemy = dynamic_cast<HookEnemy*>(e)) {
                    if (mHookEnemyState == State::Stop) {
                        if (hookEnemy->GetHookEnemyState() == State::Stop) {
                            if (mColliderComponent->Intersect(*hookEnemy->GetComponent<ColliderComponent>())) {
                                if (GetPosition().x <= hookEnemy->GetPosition().x &&
                                    mLastForwardAttackDirection == 1)
                                {
                                    SetPosition(GetPosition() + Vector2(-mHeight, 0));
                                }
                                if (GetPosition().x >= hookEnemy->GetPosition().x &&
                                    mLastForwardAttackDirection == -1)
                                {
                                    SetPosition(GetPosition() + Vector2(+mHeight, 0));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void HookEnemy::MovementBeforePlayerSpotted() {

}

void HookEnemy::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mHookEnemyState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Hook:
            Hook(deltaTime);
        break;

        case State::DiagonalAttack:
            DiagonalAttack(deltaTime);
        break;

        case State::ForwardAttack:
            ForwardAttack(deltaTime);
        break;
    }
}

void HookEnemy::Stop(float deltaTime) {
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    }
    mStopTimer += deltaTime;

    SetRotation(3 * Math::Pi / 2);
    SetTransformRotation(3 * Math::Pi / 2);
    SetScale(Vector2(1, 1));
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        float hookProbability = 0.5f;

        std::vector<Enemy* > enemies = mGame->GetEnemies();
        if (!enemies.empty()) {
            for (Enemy* e: enemies) {
                if (e != this) {
                    if (auto* hookEnemy = dynamic_cast<HookEnemy*>(e)) {
                        if (hookEnemy->GetHookEnemyState() == State::Stop) {
                           hookProbability = 0.5f;
                        }
                        else if (hookEnemy->GetHookEnemyState() == State::Hook || hookEnemy->GetHookEnemyState() == State::DiagonalAttack) {
                            hookProbability = 0.3f;
                        }
                        else if (hookEnemy->GetHookEnemyState() == State::ForwardAttack) {
                            hookProbability = 0.7f;
                        }
                    }
                }
            }
        }
        if (Random::GetFloat() < hookProbability) {
            mHookEnemyState = State::Hook;
            std::vector<HookPoint* > hookPoints = mGame->GetHookPoints();

            HookPoint* nearestHookPoint = nullptr;
            float nearestDistance = FLT_MAX;

            for (HookPoint* hp: hookPoints) {
                float dist = (GetPosition() - hp->GetPosition()).Length();
                if (dist < hp->GetRadius()) {
                    if (dist < nearestDistance) {
                        nearestDistance = dist;
                        nearestHookPoint = hp;
                    }
                }
            }

            if (nearestHookPoint) {
                mHookPoint = nearestHookPoint;
                Vector2 dir = (nearestHookPoint->GetPosition() - GetPosition());
                if (dir.Length() > 0) {
                    dir.Normalize();
                }

                mHookDirection = dir;

                // Configura o alvo final
                mHookEnd = nearestHookPoint->GetPosition();

                // A ponta da corda começa na posição do jogador
                mCurrentRopeTip = GetPosition();

                // Ativa o estado de ARREMESSO (Throwing), mas NÃO o de puxar (Hooking)
                mIsHookThrowing = true;
                mIsHooking = false; // Garante que não puxa ainda

                // Inicia a animação visual (o componente precisa ficar visível)
                mIsHookAnimating = true;
                mHookAnimProgress = 0.0f; // Reseta a ondulação da corda

                if (mDrawRopeComponent) {
                    mDrawRopeComponent->SetVisible(true);
                    mDrawRopeComponent->SetEndpoints(GetPosition(), mCurrentRopeTip);
                }
            }
        }
        else {
            Player* player = GetGame()->GetPlayer();
            float dist = GetPosition().x - player->GetPosition().x;
            if (dist < 0) {
                SetRotation(0.0);
                SetTransformRotation(0.0f);
                SetScale(Vector2(1,1));
            }
            else {
                SetRotation(Math::Pi);
                SetTransformRotation(0.0f);
                SetScale(Vector2(-1,1));
            }
            mHookEnemyState = State::ForwardAttack;
        }
    }
}

void HookEnemy::Hook(float deltaTime) {
    mHookStateTimer += deltaTime;
    if (mHookStateTimer >= mHookStateDuration) {
        mHookStateTimer = 0;
        Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
        float dx = playerPos.x - GetPosition().x;
        float dy = playerPos.y - GetPosition().y;

        float angle = Math::Atan2(dy, dx);
        // Ajustar para intervalo [0, 2*pi)
        if (angle < 0) {
            angle += Math::TwoPi;
        }
        SetRotation(angle);
        SetTransformRotation(angle);

        mHookEnemyState = State::DiagonalAttack;
    }

    if (mHookingTimer < mHookingDuration * 5) {
        // mHookingTimer += deltaTime;
    }

    if (mIsHookAnimating) {
        mHookAnimProgress += deltaTime / mHookAnimationDuration;
        if (mHookAnimProgress >= 1.0f) {
            mHookAnimProgress = 1.0f;
        }
        if (mDrawRopeComponent) {
            mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
            mDrawRopeComponent->SetAnimationProgress(mHookAnimProgress);
        }
    }

    if (mIsHookThrowing) {
        // Calcula a distância até o alvo
        float distanceToTarget = (mHookEnd - mCurrentRopeTip).Length();
        float moveStep = mRopeThrowSpeed * deltaTime;
        mHookPoint->SetHookPointState(HookPoint::HookPointState::InRange);

        if (moveStep >= distanceToTarget) {
            // A CORDA CHEGOU NO ALVO!
            mCurrentRopeTip = mHookEnd;
            mIsHookThrowing = false;

            // AGORA sim começamos a puxar o jogador
            mIsHooking = true;
            mHookingTimer = 0.0f; // Reseta o timer de puxada

            // Toca som de impacto/conectar
            // mGame->GetAudio()->PlaySound("Hook/HookHit.wav"); // Exemplo
        }
        else {
            // A corda ainda está viajando
            // Move a ponta na direção do alvo
            Vector2 travelDir = (mHookEnd - mCurrentRopeTip);
            travelDir.Normalize();
            mCurrentRopeTip += travelDir * moveStep;
        }

        // Atualiza o desenho da corda enquanto ela viaja
        if (mDrawRopeComponent) {
            // Start no Player (que pode estar caindo), End na ponta viajante
            mDrawRopeComponent->SetEndpoints(GetPosition(), mCurrentRopeTip);
        }
    }
    if (mIsHooking) {
        if (mHookingTimer < mHookingDuration) {
            mHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
            mRigidBodyComponent->SetVelocity(mHookDirection * mHookSpeed);
            mHookingTimer += deltaTime;
            // Garante que a corda fique desenhada esticada até o fim
            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
            }
        } else {
            mIsHooking = false;
            mIsHookAnimating = false;
            mHookPoint = nullptr;

            if (mDrawRopeComponent) {
                mDrawRopeComponent->SetVisible(false);
            }
        }
    }
}

void HookEnemy::DiagonalAttack(float deltaTime) {
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed * mSpeedIncreaseRate);
    }
    if (mIsOnGround) {
        Player* player = GetGame()->GetPlayer();
        float dist = GetPosition().x - player->GetPosition().x;
        if (dist < 0) {
            SetRotation(0.0);
            SetTransformRotation(0.0f);
            SetScale(Vector2(1,1));
        }
        else {
            SetRotation(Math::Pi);
            SetTransformRotation(0.0f);
            SetScale(Vector2(-1,1));
        }
        mHookEnemyState = State::ForwardAttack;
    }
}

void HookEnemy::ForwardAttack(float deltaTime) {
    mForwardAttackTimer += deltaTime;
    if (mForwardAttackTimer >= mForwardAttackDuration) {
        mForwardAttackTimer = 0;
        mForwardAttackDelayTimer = 0;
        SetRotation(3 * Math::Pi / 2);
        SetTransformRotation(3 * Math::Pi / 2);
        mHookEnemyState = State::Stop;
        return;
    }

    if (mForwardAttackDelayTimer < mForwardAttackDelayDuration) {
        mForwardAttackDelayTimer += deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }
    else if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed * mSpeedIncreaseRate);
    }
    mLastForwardAttackDirection = GetForward().x;
}

void HookEnemy::TriggerBossDefeat() {
    // Player ganha hook
    if (!mGame->GetPlayer()->GetCanHook()) {
        auto* skill = new Skill(mGame, Skill::SkillType::Hook);
        skill->SetPosition(GetPosition());
    }

    mGame->SetWorldFlag("HookEnemyDefeated", true);
    mGame->StopBossMusic();
}

void HookEnemy::ManageAnimations() {
    mDrawComponent->SetWidth(mWidth * 1.35f);
    mDrawComponent->SetHeight(mWidth * 1.35f * 0.73f);
    mDrawComponent->SetAnimFPS(7.0f);
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    if (mHookEnemyState == State::Stop) {
        mDrawComponent->SetAnimation("idle");
        float dist = GetPosition().x - playerPos.x;
        if (dist > 0) {
            SetScale(Vector2(1, -1));
        }
        else {
            SetScale(Vector2(1, 1));
        }
    }
    else if (mHookEnemyState == State::Hook) {
        if (mIsHookThrowing) {
            mDrawComponent->SetAnimation("idle");
            float dist = GetPosition().x - playerPos.x;
            if (dist > 0) {
                SetScale(Vector2(1, -1));
            }
            else {
                SetScale(Vector2(1, 1));
            }
        }
        else if (mIsHooking) {
            mDrawComponent->SetAnimation("jump");
            if (mRigidBodyComponent->GetVelocity().x < 0) {
                SetScale(Vector2(1, -1));
            }
            else {
                SetScale(Vector2(1, 1));
            }
        }
    }
    else if (mHookEnemyState == State::DiagonalAttack) {
        mDrawComponent->SetAnimation("fly");
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            SetScale(Vector2(1, -1));
        }
        else {
            SetScale(Vector2(1, 1));
        }
        mDrawComponent->SetWidth(mWidth * 1.55f);
        mDrawComponent->SetHeight(mWidth * 1.55f * 0.73f);
    }
    else if (mHookEnemyState == State::ForwardAttack) {
        mDrawComponent->SetAnimation("fly");

        mDrawComponent->SetWidth(mWidth * 1.55f);
        mDrawComponent->SetHeight(mWidth * 1.55f * 0.73f);
    }
    if (mIsFlashing) {
        mDrawComponent->SetTextureFactor(0.0f);
    }
    else {
        mDrawComponent->SetTextureFactor(1.0f);
    }
}

void HookEnemy::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;

    mHookSpeed = mHookSpeed / oldScale * newScale;
    mHookAmplitude = mHookAmplitude / oldScale * newScale;
    mHookSegmentHeight = mHookSegmentHeight / oldScale * newScale;

    mHookEnd.x = mHookEnd.x / oldScale * newScale;
    mHookEnd.y = mHookEnd.y / oldScale * newScale;

    // if (mDrawRopeComponent) {
    //     mDrawRopeComponent->SetNumSegments(mHookSegments);
    //     mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    //     mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);
    // }

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth( mWidth * 1.35f);
    //     mDrawAnimatedComponent->SetHeight(mWidth * 1.35f * 0.73f);
    // }

    if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
        obb->Update(0);
        obb->SetHalfSize(Vector2(mWidth / 2, mHeight / 2));

        // if (mDrawPolygonComponent) {
        //     auto verts = obb->GetVertices();
        //     mDrawPolygonComponent->SetVertices(verts);
        // }
    }
}
