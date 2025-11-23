#include "BushMonster.h"

#include "Effect.h"
#include "ParticleSystem.h"
#include "Skill.h"
#include "../Game.h"
#include "../HUD.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/AABBComponent.h"
#include "../Math.h"
#include "../Random.h"

BushMonster::BushMonster(Game* game)
    :Enemy(game)
    ,mBushMonsterState(State::Idle)
    ,mGravity(3000 * mGame->GetScale())
    ,mIdleDuration(1.0f)
    ,mIdleTimer(0.0f)
    ,mDashDuration(5.6f)
    ,mDashTimer(0.0f)
    ,mDashSpeed(1400.0f * mGame->GetScale())
    ,mIsDashingRight(true)
    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)
{
    mWidth = 220 * mGame->GetScale();
    mHeight = 140 * mGame->GetScale();
    mMoveSpeed = 300 * mGame->GetScale();
    mHealthPoints = 100;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 10;
    mMoneyDrop = 100;
    mFreezeMax = 1000;
    mFrozenDecayRate = mFreezeMax / 3.0f;

    SetSize(mWidth, mHeight);

    const std::string spritePath = "../Assets/Sprites/BushMonster2/BushMonster.png";
    const std::string jsonPath = "../Assets/Sprites/BushMonster2/BushMonster.json";

    mDrawComponent = new AnimatorComponent(this, spritePath,
                                            jsonPath,
                                            mWidth * 2.5f, mWidth * 2.5f / 1.29f, 998);


    mDrawComponent->AddAnimation("idle", {35, 17, 18, 19, 20, 21, 22, 36});
    mDrawComponent->AddAnimation("attack", {
        0, 1, 2, 3, 4, 5, 6, 7,
        8, 9, 10, 11, 12, 13, 14,
        30, 31, 32
    });
    mDrawComponent->AddAnimation("hit", {33, 15, 16, 34});
    mDrawComponent->AddAnimation("run", {23, 24, 25, 26, 27, 28, 29});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);
}


void BushMonster::OnUpdate(float deltaTime) {
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
    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x,
                                             mRigidBodyComponent->GetVelocity().y + mGravity * deltaTime));

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

    if (mHealthPoints <= 0.0f * mMaxHealthPoints) {
        mIdleDuration = 0.8f;
        mDashSpeed = 2000 * mGame->GetScale();
    }
    if (!mIsFrozen) {
        if (mDrawComponent) {
            ManageAnimations();
        }
    }
}

void BushMonster::ResolveGroundCollision() {
    Vector2 collisionNormal(Vector2::Zero);
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (!g->GetIsSpike()) { // Colisão com ground
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                    collisionNormal = mColliderComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
                }
                else {
                    collisionNormal = Vector2::Zero;
                }
                if (collisionNormal == Vector2::NegUnitX) {
                    if (mBushMonsterState == State::Dashing && GetRotation() == 0) {
                        mDashTimer = 0;
                        // Pode entrar em idle ou continuar dashando
                        if (Random::GetFloat() < 0.5) {
                            mIsDashingRight = false;
                        }
                        else {
                            mBushMonsterState = State::Idle;
                        }
                    }
                }
                if (collisionNormal == Vector2::UnitX){
                    if (mBushMonsterState == State::Dashing && GetRotation() == Math::Pi) {
                        mDashTimer = 0;
                        // Pode entrar em idle ou continuar dashando
                        if (Random::GetFloat() < 0.5) {
                            mIsDashingRight = true;
                        }
                        else {
                            mBushMonsterState = State::Idle;
                        }
                    }
                }
            }
            else if (g->GetIsSpike()) { // Colisão com spikes
                if (mColliderComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
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
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
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
        SetScale(Vector2(1, 1));
    }
    else {
        SetRotation(Math::Pi);
        SetScale(Vector2(-1, 1));
    }

    mRigidBodyComponent->SetVelocity(Vector2(GetForward().x * mDashSpeed, mRigidBodyComponent->GetVelocity().y));

    if (mDashTimer >= mDashDuration) {
        mDashTimer = 0.0f;
        mBushMonsterState = State::Idle;
    }
}

void BushMonster::ManageAnimations() {
    if (mIsFlashing) {
        mDrawComponent->SetAnimation("hit");
        mDrawComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else if (mBushMonsterState == State::Dashing) {
        mDrawComponent->SetAnimation("run");
        mDrawComponent->SetAnimFPS(10);
    }
    else if (mBushMonsterState == State::Idle) {
        mDrawComponent->SetAnimation("idle");
        mDrawComponent->SetAnimFPS(10);
    }
}

void BushMonster::TriggerBossDefeat() {
    // Player ganha dash
    if (!mGame->GetPlayer()->GetCanDash()) {
        auto* skill = new Skill(mGame, Skill::SkillType::Dash);
        skill->SetPosition(GetPosition());
    }

    mGame->SetWorldFlag("BushMonsterDefeated", true);
    mGame->StopBossMusic();
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth * 2.5f);
    //     mDrawAnimatedComponent->SetHeight(mWidth * 2.5f / 1.29f);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);
    std::vector<Vector2> vertices = { v1, v2, v3, v4 };

    if (auto* aabb = dynamic_cast<AABBComponent*>(mColliderComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
