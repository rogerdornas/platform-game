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
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawRopeComponent.h"

HookEnemy::HookEnemy(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 10)
    ,mHookEnemyState(State::Stop)

    ,mGravity(3000 * mGame->GetScale())
    ,mIsOnGround(true)

    ,mStopDuration(1.2f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)

    ,mHookStateDuration(0.6f)
    ,mHookStateTimer(0.0f)
    ,mIsHooking(false)
    ,mHookDirection(Vector2::Zero)
    ,mHookSpeed(1600.0f * mGame->GetScale())
    ,mHookingDuration(0.15f)
    ,mHookingTimer(0.0f)
    ,mHookEnd(Vector2::Zero)
    ,mHookAnimProgress(0.0f)
    ,mIsHookAnimating(false)
    ,mHookPoint(nullptr)
    ,mHookAnimationDuration(0.2f)
    ,mHookSegments(20)
    ,mHookAmplitude(12.0f * mGame->GetScale())
    ,mHookSegmentHeight(18.0f * mGame->GetScale())

    ,mForwardAttackDuration(1.5f)
    ,mForwardAttackTimer(0.0f)
    ,mForwardAttackDelayDuration(0.35f)
    ,mForwardAttackDelayTimer(0.0f)
    ,mLastForwardAttackDirection(1)

    ,mSpeedIncreaseRate(3.5f)
{
    mMoneyDrop = 50;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.35f, mWidth * 1.35f * 0.73f, "../Assets/Sprites/HookEnemy/HookEnemy.png", "../Assets/Sprites/HookEnemy/HookEnemy.json", 998);
    std::vector idle = {2, 3, 4, 5};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector fly = {1};
    mDrawAnimatedComponent->AddAnimation("fly", fly);

    std::vector jump = {6};
    mDrawAnimatedComponent->AddAnimation("jump", jump);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(7.0f);


    mDrawRopeComponent = new DrawRopeComponent(this, "../Assets/Sprites/Rope/Rope2.png");
    mDrawRopeComponent->SetNumSegments(mHookSegments);
    mDrawRopeComponent->SetAmplitude(mHookAmplitude);
    mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);

    RemoveComponent(mColliderComponent);
    delete mColliderComponent;
    mColliderComponent = nullptr;

    mColliderComponent = new OBBComponent(this, Vector2(mWidth / 2, mHeight / 2));

    if (mDrawPolygonComponent) {
        if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
            auto verts = obb->GetVertices();
            mDrawPolygonComponent->SetVertices(verts);
        }
    }

    SetRotation(3 * Math::Pi / 2);
}

void HookEnemy::OnUpdate(float deltaTime) {
    mIsOnGround = false;
    mKnockBackTimer += deltaTime;
    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mHookEnemyState != State::DiagonalAttack) {
            // mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    // Gravidade
    if (!mIsOnGround && mHookEnemyState != State::ForwardAttack) {
        mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                                 mRigidBodyComponent->GetVelocity().y
                                                 + mGravity * deltaTime));
    }

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

    // Se morreu
    if (Died()) {
        TriggerBossDefeat();
    }

    ManageAnimations();

    if (mHealthPoints <= mMaxHealthPoints / 2) {
        mStopDuration = 1.0f;
    }

    if (mDrawPolygonComponent) {
        if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
            auto verts = obb->GetVertices();
            mDrawPolygonComponent->SetVertices(verts);
        }
    }
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
                        }
                        else {
                            SetRotation(Math::Pi);
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
                nearestHookPoint->SetHookPointState(HookPoint::HookPointState::Hooked);
                Vector2 dir = (nearestHookPoint->GetPosition() - GetPosition());
                if (dir.Length() > 0) {
                    dir.Normalize();
                }
                mHookDirection = dir;
                mIsHooking = true;
                mHookingTimer = 0.0f;

                // Quando hook começa
                mHookEnd = nearestHookPoint->GetPosition();
                mHookAnimProgress = 0.0f;
                mIsHookAnimating = true;
                mDrawRopeComponent->SetIsVisible(true);

                mDrawRopeComponent->SetEndpoints(GetPosition(), mHookEnd);
                mDrawRopeComponent->SetAnimationProgress(mHookAnimProgress);
            }
        }
        else {
            Player* player = GetGame()->GetPlayer();
            float dist = GetPosition().x - player->GetPosition().x;
            if (dist < 0) {
                SetRotation(0.0);
            }
            else {
                SetRotation(Math::Pi);
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

        mHookEnemyState = State::DiagonalAttack;
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

    if (mIsHooking) {
        if (mHookingTimer < mHookingDuration) {
            mRigidBodyComponent->SetVelocity(mHookDirection * mHookSpeed);
        } else {
            mIsHooking = false;
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
        }
        else {
            SetRotation(Math::Pi);
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
    SetState(ActorState::Destroy);

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

void HookEnemy::ManageAnimations() {
    mDrawAnimatedComponent->SetWidth(mWidth * 1.35f);
    mDrawAnimatedComponent->SetHeight(mWidth * 1.35f * 0.73f);
    mDrawAnimatedComponent->SetAnimFPS(7.0f);
    mDrawAnimatedComponent->UseFlip(false);
    mDrawAnimatedComponent->UseRotation(true);
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    if (mHookEnemyState == State::Stop) {
        mDrawAnimatedComponent->SetAnimation("idle");
        float dist = GetPosition().x - playerPos.x;
        if (dist > 0) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
    }
    else if (mHookEnemyState == State::Hook) {
        mDrawAnimatedComponent->SetAnimation("jump");
        if (mRigidBodyComponent->GetVelocity().x < 0) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
    }
    else if (mHookEnemyState == State::DiagonalAttack || mHookEnemyState == State::ForwardAttack) {
        mDrawAnimatedComponent->SetAnimation("fly");
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
        mDrawAnimatedComponent->SetWidth(mWidth * 1.55f);
        mDrawAnimatedComponent->SetHeight(mWidth * 1.55f * 0.73f);
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

    if (mDrawRopeComponent) {
        mDrawRopeComponent->SetNumSegments(mHookSegments);
        mDrawRopeComponent->SetAmplitude(mHookAmplitude);
        mDrawRopeComponent->SetSegmentHeight(mHookSegmentHeight);
    }

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth( mWidth * 1.35f);
    mDrawAnimatedComponent->SetHeight(mWidth * 1.35f * 0.73f);

    if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
        obb->Update(0);
        obb->SetHalfSize(Vector2(mWidth / 2, mHeight / 2));

        if (mDrawPolygonComponent) {
            auto verts = obb->GetVertices();
            mDrawPolygonComponent->SetVertices(verts);
        }
    }
}
