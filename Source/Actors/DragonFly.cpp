//
// Created by roger on 06/08/2025.
//

#include "DragonFly.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

DragonFly::DragonFly(Game *game)
    :Enemy(game)
    ,mDragonFlyState(State::FlyingAround)

    ,mDistToSpotPlayer(700 * mGame->GetScale())
    ,mFlyingAroundDuration(1.0f)
    ,mFlyingAroundTimer(mFlyingAroundDuration)
    ,mFlyingAroundMoveSpeed(100.0f * mGame->GetScale())

    ,mStopDuration(0.5f)
    ,mStopTimer(0.0f)
    ,mIsCharging(false)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mAttackStraightDuration(0.3f)
    ,mAttackStraightTimer(0.0f)
    ,mAttackStraightTarget(Vector2::Zero)
    ,mDistToGoStraight(250 * mGame->GetScale())
    ,mDistToDive(40 * mGame->GetScale())

    ,mDiveDuration(0.5f)
    ,mDiveTimer(0.0f)
    ,mDiveSpeedX(1200 * mGame->GetScale())
    ,mDiveSpeedY(-2000 * mGame->GetScale())

    ,mCurveDuration(0.5f)
    ,mCurveTimer(-mCurveDuration / 2)
    ,mCurveSpeedX(700 * mGame->GetScale())
    ,mCurveSpeedY(16500 * mGame->GetScale())
    ,mIsCurvingRight(true)

    ,mStumDuration(1.5f)
    ,mStumTimer(0.0f)
    ,mStunnedAnimation(false)

    ,mAttackDuration(0.7f)
    ,mAttackTimer(0.0f)
{
    mWidth = 130 * mGame->GetScale();
    mHeight = 70 * mGame->GetScale();
    mMoveSpeed = 1300 * mGame->GetScale();
    mHealthPoints = 120;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 30;
    mKnockBackSpeed = 700.0f * mGame->GetScale();
    mKnockBackDuration = 0.15f;
    mKnockBackTimer = mKnockBackDuration;
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.8f, mWidth * 1.8f, "../Assets/Sprites/DragonFly/DragonFly.png", "../Assets/Sprites/DragonFly/DragonFly.json", 999);
    std::vector idle = {39, 40, 41, 42, 43, 44};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector hit = {35, 36, 37, 38};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    mDrawAnimatedComponent->AddAnimation("attack", attack);

    std::vector charge = {13, 14, 15, 16};
    mDrawAnimatedComponent->AddAnimation("charge", charge);

    std::vector chargeFly = {17, 18, 19, 20, 21, 22};
    mDrawAnimatedComponent->AddAnimation("chargeFly", chargeFly);

    std::vector hitStum = {23, 24, 25, 26, 27, 28};
    mDrawAnimatedComponent->AddAnimation("hitStum", hitStum);

    std::vector stumLoop = {45, 46, 47, 48, 49, 50};
    mDrawAnimatedComponent->AddAnimation("stumLoop", stumLoop);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);


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
}

void DragonFly::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;
    mFlyingAroundTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && mDragonFlyState != State::Attack &&
            mDragonFlyState != State::AttackStraight &&
            mDragonFlyState != State::Dive)
        {
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->ResetAnimationTimer();
            }
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    MovementAfterPlayerSpotted(deltaTime);

    // Se morreu
    if (Died()) {
    }

    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }
    if (mDrawPolygonComponent) {
        if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
            auto verts = obb->GetVertices();
            mDrawPolygonComponent->SetVertices(verts);
        }
    }
}

void DragonFly::ResolveGroundCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    if (mDragonFlyState != State::Attack && mDragonFlyState != State::FlyingAround) {
                        if (collisionNormal == Vector2::NegUnitX && mDragonFlyState != State::Stum) {
                            if (GetRotation() > Math::ToRadians(330) || GetRotation() < Math::ToRadians(30)) {
                                if (mDrawAnimatedComponent) {
                                    mDrawAnimatedComponent->ResetAnimationTimer();
                                }
                                mDragonFlyState = State::Stum;
                            }
                        }
                        if (collisionNormal == Vector2::UnitX && mDragonFlyState != State::Stum) {
                            if (GetRotation() > Math::ToRadians(150) && GetRotation() < Math::ToRadians(210)) {
                                if (mDrawAnimatedComponent) {
                                    mDrawAnimatedComponent->ResetAnimationTimer();
                                }
                                mDragonFlyState = State::Stum;
                            }
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    mDragonFlyState = State::Stop;
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                    // Colidiu top
                    if (collisionNormal == Vector2::NegUnitY){
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionNormal == Vector2::UnitY){
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionNormal == Vector2::NegUnitX){
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionNormal == Vector2::UnitX){
                        ReceiveHit(10, Vector2::UnitX);
                    }

                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

void DragonFly::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mDragonFlyState) {
        case State::FlyingAround:
            FlyingAround(deltaTime);
        break;

        case State::Stop:
            Stop(deltaTime);
        break;

        case State::FollowPlayer:
            FollowPlayer(deltaTime);
        break;

        case State::AttackStraight:
            AttackStraight(deltaTime);
        break;

        case State::Dive:
            Dive(deltaTime);
        break;

        case State::Curve:
            Curve(deltaTime);
        break;

        case State::Stum:
            Stum(deltaTime);
        break;

        case State::Attack:
            Attack(deltaTime);
        break;
    }
}

void DragonFly::MovementBeforePlayerSpotted() {
    Player *player = GetGame()->GetPlayer();
    if (mFlyingAroundTimer > mFlyingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mFlyingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mFlyingAroundMoveSpeed));
    }

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }
}

void DragonFly::FlyingAround(float deltaTime) {
    Player *player = GetGame()->GetPlayer();
    if (mFlyingAroundTimer > mFlyingAroundDuration) {
        SetRotation(Math::Abs(GetRotation() - Math::Pi)); // Comuta rotação entre 0 e Pi
        mFlyingAroundTimer = 0;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(GetForward() * mFlyingAroundMoveSpeed));
    }

    // Testa se spotted player
    Vector2 dist = GetPosition() - player->GetPosition();
    if (dist.Length() < mDistToSpotPlayer) {
        mPlayerSpotted = true;
    }

    if (mPlayerSpotted) {
        mDragonFlyState = State::Stop;
    }
}

void DragonFly::Stop(float deltaTime) {
    Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();
    float dist = GetPosition().x - playerPos.x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }

    if (mStopTimer >= mStopDuration * 0.5f) {
        if (mIsCharging == false) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mIsCharging = true;
    }

    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        mIsCharging = false;
        mDragonFlyState = State::FollowPlayer;
    }
}

void DragonFly::FollowPlayer(float deltaTime) {
    Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

    float dx = playerPos.x - GetPosition().x;
    float dy = playerPos.y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += Math::TwoPi;
    }

    SetRotation(angle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
    }

    float dist = (GetPosition() -playerPos).Length();

    if (dist <= mDistToGoStraight) {
        mAttackStraightTarget = playerPos;
        Vector2 direction = playerPos - GetPosition();
        if (direction.Length() > 0) {
            direction.Normalize();
        }
        SetRotation(Math::Atan2(direction.y, direction.x));
        mDragonFlyState = State::AttackStraight;
    }
}


void DragonFly::AttackStraight(float deltaTime) {
    mAttackStraightTimer += deltaTime;
    if (mAttackStraightTimer >= mAttackStraightDuration) {
        mAttackStraightTimer = 0;
        mDragonFlyState = State::Dive;
        return;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed);
    }

    float angle = Math::Atan2(mRigidBodyComponent->GetVelocity().y, mRigidBodyComponent->GetVelocity().x);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += Math::TwoPi;
    }
    SetRotation(angle);

    float dist = (GetPosition() - mAttackStraightTarget).Length();
    if (dist <= mDistToDive) {
        mAttackStraightTimer = 0;
        mDragonFlyState = State::Dive;
    }
}

void DragonFly::Dive(float deltaTime) {
    mDiveTimer += deltaTime;
    if (mDiveTimer >= mDiveDuration) {
        mDiveTimer = 0;
        mDragonFlyState = State::Curve;
        return;
    }

    float t = mDiveTimer / mDiveDuration;
    float targetYVelocity = mDiveSpeedY * t * t;
    if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
        mRigidBodyComponent->SetVelocity(Vector2(-mDiveSpeedX, targetYVelocity));
        mIsCurvingRight = true;
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2(mDiveSpeedX, targetYVelocity));
        mIsCurvingRight = false;
    }

    float angle = Math::Atan2(mRigidBodyComponent->GetVelocity().y, mRigidBodyComponent->GetVelocity().x);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += Math::TwoPi;
    }
    SetRotation(angle);
}

void DragonFly::Curve(float deltaTime) {
    mCurveTimer += deltaTime;
    if (mCurveTimer >= mCurveDuration / 2) {
        mCurveTimer = -mCurveDuration / 2;
        mDragonFlyState = State::FollowPlayer;
        return;
    }

    float t = mCurveTimer / mCurveDuration;
    float targetYVelocity = mCurveSpeedY * t * t * t;
    if (mIsCurvingRight) {
        mRigidBodyComponent->SetVelocity(Vector2(mCurveSpeedX, targetYVelocity));
    }
    else {
        mRigidBodyComponent->SetVelocity(Vector2(-mCurveSpeedX, targetYVelocity));
    }

    float angle = Math::Atan2(mRigidBodyComponent->GetVelocity().y, mRigidBodyComponent->GetVelocity().x);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += Math::TwoPi;
    }
    SetRotation(angle);
}

void DragonFly::Stum(float deltaTime) {
    mStumTimer += deltaTime;
    if (mStumTimer >= mStumDuration) {
        mStumTimer = 0;
        mStunnedAnimation = false;
        SetRotation(Math::Abs(GetRotation() - Math::Pi));  // Inverte rotação
        if (mDrawAnimatedComponent) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mDragonFlyState = State::Attack;
        return;
    }
    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }

    if (mStumTimer >= mStumDuration * 0.2f) {
        mStunnedAnimation = true;
    }
}

void DragonFly::Attack(float deltaTime) {
    mAttackTimer += deltaTime;
    if (mAttackTimer >= mAttackDuration) {
        mAttackTimer = 0;
        mDragonFlyState = State::Stop;
    }

    Vector2 playerPos = GetGame()->GetPlayer()->GetPosition();

    float dx = playerPos.x - GetPosition().x;
    float dy = playerPos.y - GetPosition().y;

    float angle = Math::Atan2(dy, dx);
    // Ajustar para intervalo [0, 2*pi)
    if (angle < 0) {
        angle += Math::TwoPi;
    }

    SetRotation(angle);

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(GetForward() * mMoveSpeed * 0.5f);
    }
}

void DragonFly::ManageAnimations() {
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
    mDrawAnimatedComponent->UseFlip(false);
    mDrawAnimatedComponent->UseRotation(false);

    if (mDragonFlyState == State::FollowPlayer ||
        mDragonFlyState == State::AttackStraight)
    {
        mDrawAnimatedComponent->SetAnimation("charge");
        mDrawAnimatedComponent->SetAnimFPS(13.0f);
        mDrawAnimatedComponent->UseRotation(true);
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
    }
    else if (mDragonFlyState == State::Dive ||
             mDragonFlyState == State::Curve)
    {
        mDrawAnimatedComponent->SetAnimation("idle");
        mDrawAnimatedComponent->SetAnimFPS(13.0f);
        mDrawAnimatedComponent->UseRotation(true);
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
    }
    else if (mDragonFlyState == State::Stum) {
        mDrawAnimatedComponent->UseRotation(true);
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
        if (mStunnedAnimation) {
            mDrawAnimatedComponent->SetAnimation("stumLoop");
        }
        else {
            mDrawAnimatedComponent->SetAnimation("hitStum");
            mDrawAnimatedComponent->SetAnimFPS(6.0f / (mStumDuration * 0.2));
        }
    }
    else if (mDragonFlyState == State::Attack) {
        mDrawAnimatedComponent->UseRotation(true);
        if (GetRotation() > Math::PiOver2 && GetRotation() < 3 * Math::PiOver2) {
            mDrawAnimatedComponent->UseFlip(true);
            mDrawAnimatedComponent->SetFlip(SDL_FLIP_VERTICAL);
        }
        mDrawAnimatedComponent->SetAnimation("attack");
        mDrawAnimatedComponent->SetAnimFPS(13.0f / mAttackDuration);
    }
    else if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else if (mIsCharging) {
        mDrawAnimatedComponent->SetAnimation("chargeFly");
        mDrawAnimatedComponent->SetAnimFPS(6.0f / (mStopDuration * 0.5f));
    }
    else if (mDragonFlyState == State::Stop || mDragonFlyState == State::FlyingAround) {
        mDrawAnimatedComponent->SetAnimation("idle");
    }
}

void DragonFly::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mDistToSpotPlayer = mDistToSpotPlayer / oldScale * newScale;
    mFlyingAroundMoveSpeed = mFlyingAroundMoveSpeed / oldScale * newScale;
    mAttackStraightTarget.x = mAttackStraightTarget.x / oldScale * newScale;
    mAttackStraightTarget.y = mAttackStraightTarget.y / oldScale * newScale;
    mDistToGoStraight = mDistToGoStraight / oldScale * newScale;
    mDistToDive = mDistToDive / oldScale * newScale;
    mDiveSpeedX = mDiveSpeedX / oldScale * newScale;
    mDiveSpeedY = mDiveSpeedY / oldScale * newScale;
    mCurveSpeedX = mCurveSpeedX / oldScale * newScale;
    mCurveSpeedY = mCurveSpeedY / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
        mDrawAnimatedComponent->SetHeight(mWidth * 1.8f);
    }

    if (auto* obb = dynamic_cast<OBBComponent*>(mColliderComponent)) {
        obb->Update(0);
        obb->SetHalfSize(Vector2(mWidth / 2, mHeight / 2));

        if (mDrawPolygonComponent) {
            auto verts = obb->GetVertices();
            mDrawPolygonComponent->SetVertices(verts);
        }
    }
}




