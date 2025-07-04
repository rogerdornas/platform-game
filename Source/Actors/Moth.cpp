//
// Created by roger on 25/06/2025.
//

#include "Moth.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Projectile.h"
#include "Skill.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/DynamicGround.h"

Moth::Moth(Game *game, float width, float height, float moveSpeed, float healthPoints)
    :Enemy(game, width, height, moveSpeed, healthPoints, 15)
    ,mMothState(State::Stop)
    ,mIsSlowMotion(false)
    ,mSlowMotionProbability(0.45f)

    ,mStopDuration(2.0f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)
    ,mHitTimer(0.0f)

    ,mDyingDuration(2.0f)
    ,mDyingTimer(0.0f)

    ,mBoostUpDuration(2.5f)
    ,mBoostUpTimer(0.0f)
    ,mAlreadyBoosted(false)

    ,mAttackDuration(0.9f)
    ,mAttackTimer(0.0f)
    ,mAttackAnimation(false)

    ,mProjectileWidth(60 * mGame->GetScale())
    ,mProjectileHeight(60 * mGame->GetScale())
    ,mProjectileSpeed(1400 * mGame->GetScale())
    ,mMaxProjectiles(15)
    ,mCountProjectiles(0)
    ,mDurationBetweenProjectiles(0.2f)
    ,mTimerBetweenProjectiles(0.0f)

    ,mProjectilesStartDirection(0)

    ,mCircleProjectileWidth(60 * mGame->GetScale())
    ,mCircleProjectileHeight(60 * mGame->GetScale())
    ,mCircleProjectileSpeed(800 * mGame->GetScale())
    ,mMaxCircleProjectiles(40)
    ,mCountCircleProjectiles(0)
    ,mCircleProjectilesDuration(1.5f)
    ,mCircleProjectilesTimer(0.0f)

    ,mFlyUpDuration(6.5f)
    ,mFlyUpTimer(0.0f)
{
    mMoneyDrop = 200;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    // mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Moth/Moth2.png", mWidth * 1.3f, mHeight * 1.3f, 999);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 2.0f, mHeight * 2.0f, "../Assets/Sprites/FinalBoss/FinalBoss.png", "../Assets/Sprites/FinalBoss/FinalBoss.json", 999);
    std::vector idle = {53, 54, 55, 56, 57, 58, 59, 60};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector attack = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    mDrawAnimatedComponent->AddAnimation("attack", attack);

    std::vector boostUp = {14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};
    mDrawAnimatedComponent->AddAnimation("boostUp", boostUp);

    std::vector die = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48};
    mDrawAnimatedComponent->AddAnimation("die", die);

    std::vector hit = {49, 50, 51, 52};
    mDrawAnimatedComponent->AddAnimation("hit", hit);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
}

void Moth::OnUpdate(float deltaTime) {
    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0 && !mAttackAnimation && mMothState != State::BoostUp) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
    }

    Player* player = GetGame()->GetPlayer();
    float dist = GetPosition().x - player->GetPosition().x;
    if (dist < 0) {
        SetRotation(0.0);
    }
    else {
        SetRotation(Math::Pi);
    }

    // Revida bola de fogo
    std::vector<FireBall*> fireBalls = mGame->GetFireBalls();
    if (!fireBalls.empty()) {
        for (FireBall* f : fireBalls) {
            if ((GetPosition() - f->GetPosition()).Length() < 150 * mGame->GetScale()) {
                if (Random::GetFloat() < mSlowMotionProbability) {
                    Vector2 direction = player->GetPosition() - GetPosition();
                    if (direction.Length() > 0) {
                        direction.Normalize();
                    }
                    f->SetRotation(Math::Atan2(direction.y, direction.x));
                    f->SetIsFromEnemy();
                    f->GetComponent<DrawAnimatedComponent>()->UseRotation(true);
                    f->GetComponent<RigidBodyComponent>()->SetVelocity(f->GetForward() * f->GetSpeed());
                }
            }
        }
    }

    ResolveGroundCollision();
    ResolveEnemyCollision();

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
        // mMothState = State::Dying;
        TriggerBossDefeat();
    }

    ChangeGround(deltaTime);

    ManageAnimations();
}

void Moth::MovementBeforePlayerSpotted() {

}

void Moth::MovementAfterPlayerSpotted(float deltaTime) {
    switch (mMothState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Projectiles:
            Projectiles(deltaTime);
        break;

        case State::SlowMotionProjectiles:
            SlowMotionProjectiles(deltaTime);
        break;

        case State::CircleProjectiles:
            CircleProjectiles(deltaTime);
        break;

        case State::BoostUp:
            BoostUp(deltaTime);
        break;

        case State::FlyUp:
            FlyUp(deltaTime);
        break;

        case State::Dying:
            Dying(deltaTime);
        break;
    }
}

void Moth::Stop(float deltaTime) {
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;

        if (!mAlreadyBoosted) {
            if (mHealthPoints <= 0.5f * mMaxHealthPoints) {
                mDrawAnimatedComponent->ResetAnimationTimer();
                mAlreadyBoosted = true;
                mMothState = State::BoostUp;
                return;
            }
        }

        if (mHealthPoints > 0.88f * mMaxHealthPoints) {
            if (GetRotation() == 0) {
                mProjectilesStartDirection = 0;
            }
            if (GetRotation() == Math::Pi) {
                mProjectilesStartDirection = 1;
            }
            if (Random::GetFloat() < mSlowMotionProbability) {
                mIsSlowMotion = true;
                mGame->SetIsSlowMotion(true);
                mGame->GetAudio()->PlaySound("SlowMotion/SlowMotion.wav");
            }
            mMothState = State::Projectiles;
            return;
        }

        if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.88f * mMaxHealthPoints) {
            if (Random::GetFloat() < mSlowMotionProbability) {
                mIsSlowMotion = true;
                mGame->SetIsSlowMotion(true);
                mGame->GetAudio()->PlaySound("SlowMotion/SlowMotion.wav");
            }
            mMothState = State::CircleProjectiles;
            return;
        }

        if (mHealthPoints > 0.5f * mMaxHealthPoints && mHealthPoints <= 0.6f * mMaxHealthPoints) {
            if (GetRotation() == 0) {
                mProjectilesStartDirection = 0;
            }
            if (GetRotation() == Math::Pi) {
                mProjectilesStartDirection = 1;
            }
            if (Random::GetFloat() < mSlowMotionProbability) {
                mIsSlowMotion = true;
                mGame->SetIsSlowMotion(true);
                mGame->GetAudio()->PlaySound("SlowMotion/SlowMotion.wav");
            }
            mMothState = State::Projectiles;
            return;
        }

        if (mHealthPoints <= 0.5f * mMaxHealthPoints) {
            if (Random::GetFloat() < 0.5f) {
                if (GetRotation() == 0) {
                    mProjectilesStartDirection = 0;
                }
                if (GetRotation() == Math::Pi) {
                    mProjectilesStartDirection = 1;
                }
                if (Random::GetFloat() < mSlowMotionProbability) {
                    mIsSlowMotion = true;
                    mGame->SetIsSlowMotion(true);
                    mGame->GetAudio()->PlaySound("SlowMotion/SlowMotion.wav");
                }
                mMothState = State::Projectiles;
            }
            else {
                if (Random::GetFloat() < mSlowMotionProbability) {
                    mIsSlowMotion = true;
                    mGame->SetIsSlowMotion(true);
                    mGame->GetAudio()->PlaySound("SlowMotion/SlowMotion.wav");
                }
                mMothState = State::CircleProjectiles;
            }
        }
    }
}

void Moth::Projectiles(float deltaTime) {
    if (mAttackTimer == 0) {
        mDrawAnimatedComponent->ResetAnimationTimer();
    }
    mAttackTimer += deltaTime;
    if (mAttackTimer < mAttackDuration) {
        mAttackAnimation = true;
        return;
    }

    mAttackAnimation = false;

    mTimerBetweenProjectiles += deltaTime;
    if (mCountProjectiles >= mMaxProjectiles) {
        mCountProjectiles = 0;
        mAttackTimer = 0;
        mGame->SetIsSlowMotion(false);
        mIsSlowMotion = false;
        mMothState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mTimerBetweenProjectiles >= mDurationBetweenProjectiles) {
        mTimerBetweenProjectiles -= mDurationBetweenProjectiles;

        float direction;
        if (mProjectilesStartDirection == 0) {
            direction = Math::Pi * (static_cast<float>(mCountProjectiles) / mMaxProjectiles);
        }
        else {
            direction = Math::Pi * (1 - static_cast<float>(mCountProjectiles) / mMaxProjectiles);
        }

        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused && p->GetProjectileType() == Projectile::ProjectileType::OrangeBall) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                if (mIsSlowMotion) {
                    p->SetSpeed(2 * mProjectileSpeed);
                }
                else {
                    p->SetSpeed(mProjectileSpeed);
                }
                p->SetDamage(15);
                p->SetPosition(GetPosition());
                p->GetComponent<DrawAnimatedComponent>()->UseRotation(true);
                break;
            }
        }
        mCountProjectiles++;
    }
}

void Moth::SlowMotionProjectiles(float deltaTime) {
    mGame->SetIsSlowMotion(true);
    if (mAttackTimer == 0) {
        mDrawAnimatedComponent->ResetAnimationTimer();
    }
    mAttackTimer += deltaTime;
    if (mAttackTimer < mAttackDuration) {
        mAttackAnimation = true;
        return;
    }

    mAttackAnimation = false;

    // mGame->SetIsSlowMotion(true);

    mTimerBetweenProjectiles += deltaTime;
    if (mCountProjectiles >= mMaxProjectiles) {
        mCountProjectiles = 0;
        mGame->SetIsSlowMotion(false);
        mMothState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mTimerBetweenProjectiles >= mDurationBetweenProjectiles) {
        mTimerBetweenProjectiles -= mDurationBetweenProjectiles;

        float direction;
        if (mProjectilesStartDirection == 0) {
            direction = Math::Pi * (static_cast<float>(mCountProjectiles) / mMaxProjectiles);
        }
        else {
            direction = Math::Pi * (1 - static_cast<float>(mCountProjectiles) / mMaxProjectiles);
        }

        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused && p->GetProjectileType() == Projectile::ProjectileType::OrangeBall) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed * 2);
                p->SetDamage(15);
                p->SetPosition(GetPosition());
                p->GetComponent<DrawAnimatedComponent>()->UseRotation(true);
                break;
            }
        }
        mCountProjectiles++;
    }
}

void Moth::CircleProjectiles(float deltaTime) {
    mCircleProjectilesTimer += deltaTime;
    if (mCircleProjectilesTimer >= mCircleProjectilesDuration) {
        mCircleProjectilesTimer = 0;
        mCountCircleProjectiles = 0;
        mAttackTimer = 0;
        mGame->SetIsSlowMotion(false);
        mIsSlowMotion = false;
        mMothState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mAttackTimer == 0) {
        mDrawAnimatedComponent->ResetAnimationTimer();
    }
    mAttackTimer += deltaTime;
    if (mAttackTimer < mAttackDuration) {
        mAttackAnimation = true;
        return;
    }

    mAttackAnimation = false;

    for (; mCountCircleProjectiles < mMaxCircleProjectiles; mCountCircleProjectiles++) {
        float direction = Math::TwoPi * (1.0f - static_cast<float>(mCountCircleProjectiles) / (mMaxCircleProjectiles));
        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused && p->GetProjectileType() == Projectile::ProjectileType::OrangeBall) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mCircleProjectileWidth);
                p->SetHeight(mCircleProjectileHeight);
                if (mIsSlowMotion) {
                    p->SetSpeed(1.5f * mCircleProjectileSpeed);
                }
                else {
                    p->SetSpeed(mCircleProjectileSpeed);
                }
                p->SetDamage(15);
                p->SetPosition(GetPosition());
                p->GetComponent<DrawAnimatedComponent>()->UseRotation(true);
                break;
            }
        }
    }
}

void Moth::BoostUp(float deltaTime) {
    mBoostUpTimer += deltaTime;
    if (mBoostUpTimer >= mBoostUpDuration) {
        mBoostUpTimer = 0;
        mMothState = State::FlyUp;
        return;
    }
    // for (int id : {187}) {
    for (int id : {20, 32, 33}) {
        Ground *g = mGame->GetGroundById(id);
        DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
        if (dynamicGround) {
            dynamicGround->SetIsDecreasing(true);
        }
    }
}


void Moth::FlyUp(float deltaTime) {
    mFlyUpTimer += deltaTime;
    if (mFlyUpTimer >= mFlyUpDuration) {
        mFlyUpTimer = 0;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mMothState = State::Stop;
        return;
    }

    // for (int id : {174, 176, 183, 184, 192, 193, 194}) {
    for (int id : {16, 17, 18, 19, 22, 23, 24, 43}) {
        Ground *g = mGame->GetGroundById(id);
        DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
        if (dynamicGround) {
            dynamicGround->SetIsGrowing(true);
        }
    }

    mRigidBodyComponent->SetVelocity(Vector2(0, -mMoveSpeed));
}

void Moth::Dying(float deltaTime) {
    mDyingTimer += deltaTime;
    if (mDyingTimer >= mDyingDuration) {
        mDyingTimer = 0;
        TriggerBossDefeat();
    }
}


void Moth::ChangeGround(float deltaTime) {
    std::vector<Ground *> grounds = mGame->GetGrounds();

    if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.9f * mMaxHealthPoints) {
        // for (int id : {174, 176}) {
        for (int id : {16, 17}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsGrowing(true);
            }
        }
    }

    if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.86f * mMaxHealthPoints) {
        // for (int id : {172}) {
        for (int id : {15}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsGrowing(true);
            }
        }
    }

    if (mHealthPoints > 0.5f * mMaxHealthPoints && mHealthPoints <= 0.6f * mMaxHealthPoints) {
        // for (int id : {172, 174, 176}) {
        for (int id : {15, 16, 17}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsDecreasing(true);
            }
        }
    }
}


void Moth::TriggerBossDefeat() {
    for (int id : {51}) {
        Ground *g = mGame->GetGroundById(id);
        DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
        if (dynamicGround) {
            dynamicGround->SetIsGrowing(true);
        }
    }

    SetState(ActorState::Destroy);
    mGame->SetIsSlowMotion(false);

    mGame->GetCamera()->StartCameraShake(0.5, mCameraShakeStrength);

    // Player ganha controle sobre o tempo
    auto* skill = new Skill(mGame, Skill::SkillType::TimeControl);
    skill->SetPosition(GetPosition());

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

void Moth::ManageAnimations() {
    if (mAttackAnimation) {
        mDrawAnimatedComponent->SetAnimation("attack");
        mDrawAnimatedComponent->SetAnimFPS(10.0f / mAttackDuration);
    }
    else if (mMothState == State::BoostUp) {
        mDrawAnimatedComponent->SetAnimation("boostUp");
        mDrawAnimatedComponent->SetAnimFPS(22.0f / mBoostUpDuration);
    }
    else if (mMothState == State::Dying) {
        mDrawAnimatedComponent->SetAnimation("die");
        mDrawAnimatedComponent->SetAnimFPS(13.0f / mDyingDuration);
    }
    else if (mIsFlashing) {
        mDrawAnimatedComponent->SetAnimation("hit");
        mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    }
    else {
        mDrawAnimatedComponent->SetAnimation("idle");
        mDrawAnimatedComponent->SetAnimFPS(10.0f);
    }












    // if (mMothState == State::Projectiles || mMothState == State::SlowMotionProjectiles) {
    // if (mAttackAnimation) {
    //     mDrawAnimatedComponent->SetAnimation("attack");
    //     mDrawAnimatedComponent->SetAnimFPS(10.0f / mAttackDuration);
    // }
    // else if (mMothState != State::BoostUp) {
    //     mDrawAnimatedComponent->SetAnimation("idle");
    //     mDrawAnimatedComponent->SetAnimFPS(10.0f);
    // }
    //
    // if (mIsFlashing) {
    //     mDrawAnimatedComponent->SetAnimation("hit");
    //     mDrawAnimatedComponent->SetAnimFPS(4.0f / mHitDuration);
    // }
    //
    // if (mMothState == State::BoostUp) {
    //     mDrawAnimatedComponent->SetAnimation("boostUp");
    //     mDrawAnimatedComponent->SetAnimFPS(22.0f / mBoostUpDuration);
    // }

}

void Moth::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    mProjectileWidth = mProjectileWidth / oldScale * newScale;
    mProjectileHeight = mProjectileHeight / oldScale * newScale;
    mProjectileSpeed = mProjectileSpeed / oldScale * newScale;
    mCircleProjectileWidth = mCircleProjectileWidth / oldScale * newScale;
    mCircleProjectileHeight = mCircleProjectileHeight / oldScale * newScale;
    mCircleProjectileSpeed = mCircleProjectileSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    mDrawAnimatedComponent->SetWidth(mWidth * 2.0f);
    mDrawAnimatedComponent->SetHeight(mHeight * 2.0f);

    Vector2 v1(-mHeight / 2, -mHeight / 2);
    Vector2 v2(mHeight / 2, -mHeight / 2);
    Vector2 v3(mHeight / 2, mHeight / 2);
    Vector2 v4(-mHeight / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}




