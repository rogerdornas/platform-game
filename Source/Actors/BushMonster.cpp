#include "BushMonster.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Components/AABBComponent.h"
#include "../Math.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"

BushMonster::BushMonster(Game* game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 5)
    ,mBushMonsterState(State::Idle)
    ,mGravity(3000 * mGame->GetScale())
    ,mIdleDuration(1.5f)
    ,mIdleTimer(0.0f)
    ,mDashDuration(5.6f)
    ,mDashTimer(0.0f)
    ,mDashSpeed(1400.0f * mGame->GetScale())
    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)
{
    mMoneyDrop = 100;

    const std::string spritePath = "../Assets/Sprites/BushMonster2/BushMonster.png";
    const std::string jsonPath = "../Assets/Sprites/BushMonster2/BushMonster.json";

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.5f, mWidth * 2.5f / 1.29f,
                                                       spritePath, jsonPath, 998);


    mDrawAnimatedComponent->AddAnimation("idle", {35, 17, 18, 19, 20, 21, 22, 36});
    mDrawAnimatedComponent->AddAnimation("attack", {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14,
        30, 31, 32
    });
    mDrawAnimatedComponent->AddAnimation("hit", {33, 15, 16, 34});
    mDrawAnimatedComponent->AddAnimation("run", {23, 24, 25, 26, 27, 28, 29});

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
}


void BushMonster::OnUpdate(float deltaTime) {
    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

    // Player* player = GetGame()->GetPlayer();
    // float dist = GetPosition().x - player->GetPosition().x;
    // if (dist < 0) {
    //     SetRotation(0.0);
    // }
    // else {
    //     SetRotation(Math::Pi);
    // }

    // Gravidade
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y + mGravity * deltaTime));

    if (mPlayerSpotted) {
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
}

void BushMonster::ResolveGroundCollision() {
    std::array<bool, 4> collisionSide{};
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                }
                else {
                    collisionSide = {false, false, false, false};
                }
                if (collisionSide[2]) {
                    if (mBushMonsterState == State::Dashing && GetRotation() == 0) {
                        mDashTimer = 0;
                        mBushMonsterState = State::Idle;
                    }
                }
                if (collisionSide[3]) {
                    // SDL_Log("colidiu parede");
                    if (mBushMonsterState == State::Dashing && GetRotation() == Math::Pi) {
                        mDashTimer = 0;
                        mBushMonsterState = State::Idle;
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                    collisionSide = mAABBComponent->ResolveCollision(*g->GetComponent<AABBComponent>());
                    // Colidiu top
                    if (collisionSide[0]) {
                        ReceiveHit(10, Vector2::NegUnitY);
                    }
                    // Colidiu bot
                    if (collisionSide[1]) {
                        ReceiveHit(10, Vector2::UnitY);
                    }
                    //Colidiu left
                    if (collisionSide[2]) {
                        ReceiveHit(10, Vector2::NegUnitX);
                    }
                    //Colidiu right
                    if (collisionSide[3]) {
                        ReceiveHit(10, Vector2::UnitX);
                    }

                    mKnockBackTimer = 0;
                }
            }
        }
    }
}

void BushMonster::MovementBeforePlayerSpotted() {

}

void BushMonster::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mBushMonsterState) {
        case State::Idle:
            HandleIdle(deltaTime);
        break;

        case State::Dashing:
            HandleDash(deltaTime);
        break;
    }
}



void BushMonster::HandleIdle(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, mRigidBodyComponent->GetVelocity().y));
    mIdleTimer += deltaTime;

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    if (mIdleTimer >= mIdleDuration) {
        mIdleTimer = 0.0f;
        if (GetRotation() == 0) {
            mIsDashingRight = true;
        }
        else {
            mIsDashingRight = false;
        }
        mBushMonsterState = State::Dashing;
    }
}

void BushMonster::HandleDash(float deltaTime) {
    mDashTimer += deltaTime;
    if (mIsDashingRight) {
        SetRotation(0);
    }
    else {
        SetRotation(Math::Pi);
    }

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mDashSpeed, mRigidBodyComponent->GetVelocity().y));

    if (mDashTimer >= mDashDuration) {
        mDashTimer = 0.0f;
        mBushMonsterState = State::Idle;
    }
}

void BushMonster::ManageAnimations() {
    if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else if (mBushMonsterState == State::Dashing) {
        mDrawAnimatedComponent->SetAnimation("run");
        mDrawAnimatedComponent->SetAnimFPS(10);
    }
    else if (mBushMonsterState == State::Idle) {
        mDrawAnimatedComponent->SetAnimation("idle");
        mDrawAnimatedComponent->SetAnimFPS(10);
    }
    //
    // if (!mDrawAnimatedComponent) return;
    //
    // if (mIsFlashing) {
    //     if (mDrawAnimatedComponent->GetCurrentAnimName() != "hit") {
    //         mDrawAnimatedComponent->SetAnimation("hit");
    //         mDrawAnimatedComponent->SetAnimFPS(10.0f);
    //     }
    // }
    // else if (mState == State::Dashing) {
    //     if (mDrawAnimatedComponent->GetCurrentAnimName() != "attack") {
    //         mDrawAnimatedComponent->SetAnimation("attack");
    //         mDrawAnimatedComponent->SetAnimFPS(18.0f / mDashDuration);
    //     }
    // }
    // else {
    //     if (mDrawAnimatedComponent->GetCurrentAnimName() != "idle") {
    //         mDrawAnimatedComponent->SetAnimation("idle");
    //         mDrawAnimatedComponent->SetAnimFPS(10.0f);
    //     }
    // }
}

void BushMonster::TriggerBossDefeat() {

}


void BushMonster::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    mDashSpeed = mDashSpeed / oldScale * newScale;
    mGravity = mGravity / oldScale * newScale;

    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    mRigidBodyComponent->SetVelocity(Vector2(
        mRigidBodyComponent->GetVelocity().x / oldScale * newScale,
        mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 2.5f);
        mDrawAnimatedComponent->SetHeight(mWidth * 2.5f / 1.29f);
    }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);
    std::vector<Vector2> vertices = { v1, v2, v3, v4 };

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
