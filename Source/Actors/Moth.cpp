//
// Created by roger on 25/06/2025.
//

#include "Moth.h"
#include "Actor.h"
#include "Effect.h"
#include "ParticleSystem.h"
#include "Projectile.h"
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
    ,mStopDuration(2.0f)
    ,mStopTimer(0.0f)

    ,mProjectileWidth(50 * mGame->GetScale())
    ,mProjectileHeight(55 * mGame->GetScale())
    ,mProjectileSpeed(1400 * mGame->GetScale())
    ,mMaxProjectiles(15)
    ,mCountProjectiles(0)
    ,mDurationBetweenProjectiles(0.2f)
    ,mTimerBetweenProjectiles(0.0f)

    ,mCircleProjectileWidth(50 * mGame->GetScale())
    ,mCircleProjectileHeight(50 * mGame->GetScale())
    ,mCircleProjectileSpeed(800 * mGame->GetScale())
    ,mMaxCircleProjectiles(40)
    ,mCountCircleProjectiles(0)
    ,mCircleProjectilesDuration(1.5f)
    ,mCircleProjectilesTimer(0.0f)

    ,mFlyUpDuration(4.0f)
    ,mFlyUpTimer(0.0f)
{
    mMoneyDrop = 200;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;

    mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Moth/Moth2.png", mWidth * 1.3f, mHeight * 1.3f, 999);

}

void Moth::OnUpdate(float deltaTime) {

    if (mFlashTimer < mFlashDuration) {
        mFlashTimer += deltaTime;
    }
    else {
        mIsFlashing = false;
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
        TriggerBossDefeat();
    }

    ManageAnimations();

    if (mHealthPoints <= mMaxHealthPoints / 2) {

    }
    ChangeGround(deltaTime);

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

        case State::FlyUp:
            FlyUp(deltaTime);
        break;
    }
}

void Moth::Stop(float deltaTime) {
    SDL_Log("stop");
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    mStopTimer += deltaTime;
    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;
        if (mHealthPoints > 0.8f * mMaxHealthPoints && mHealthPoints <= 0.88f * mMaxHealthPoints) {
            mMothState = State::CircleProjectiles;
        }
        else {
            if (Random::GetFloat() < 0.7) {
                mMothState = State::Projectiles;
            }
            else {
                mMothState = State::SlowMotionProjectiles;
            }
        }
        return;
    }
}

void Moth::Projectiles(float deltaTime) {
    mTimerBetweenProjectiles += deltaTime;
    if (mCountProjectiles >= mMaxProjectiles) {
        mCountProjectiles = 0;
        mMothState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mTimerBetweenProjectiles >= mDurationBetweenProjectiles) {
        mTimerBetweenProjectiles -= mDurationBetweenProjectiles;

        float direction = Math::Pi * (1 - static_cast<float>(mCountProjectiles) / mMaxProjectiles);

        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed);
                p->SetDamage(15);
                p->SetPosition(GetPosition());
                break;
            }
        }
        mCountProjectiles++;
    }
}

void Moth::SlowMotionProjectiles(float deltaTime) {
    mGame->SetIsSlowMotion(true);

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

        float direction = Math::Pi * (1 - static_cast<float>(mCountProjectiles) / mMaxProjectiles);

        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed * 2);
                p->SetDamage(15);
                p->SetPosition(GetPosition());
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
        mMothState = State::Stop;
        return;
    }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mCircleProjectilesTimer >= mCircleProjectilesDuration / 2) {
        for (; mCountCircleProjectiles < mMaxCircleProjectiles; mCountCircleProjectiles++) {
            float direction = Math::TwoPi * (1.0f - static_cast<float>(mCountCircleProjectiles) / (mMaxCircleProjectiles));
            std::vector<Projectile* > projectiles = mGame->GetProjectiles();
            for (Projectile* p: projectiles) {
                if (p->GetState() == ActorState::Paused) {
                    p->SetState(ActorState::Active);
                    p->SetRotation(direction);
                    p->SetWidth(mCircleProjectileWidth);
                    p->SetHeight(mCircleProjectileHeight);
                    p->SetSpeed(mCircleProjectileSpeed);
                    p->SetDamage(15);
                    p->SetPosition(GetPosition());
                    break;
                }
            }
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

    mRigidBodyComponent->SetVelocity(Vector2(0, -mMoveSpeed));
}

void Moth::ChangeGround(float deltaTime) {
    std::vector<Ground *> grounds = mGame->GetGrounds();

    if (mHealthPoints > 0.88f * mMaxHealthPoints && mHealthPoints <= 0.9f * mMaxHealthPoints) {
        for (int id : {174, 176}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsGrowing(true);
            }
        }
    }

    if (mHealthPoints > 0.8f * mMaxHealthPoints && mHealthPoints <= 0.88f * mMaxHealthPoints) {
        for (int id : {172}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsGrowing(true);
            }
        }
    }

    if (mHealthPoints > 0.7f * mMaxHealthPoints && mHealthPoints <= 0.8f * mMaxHealthPoints) {
        for (int id : {172, 174, 176}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsDecreasing(true);
            }
        }
    }

    if (mHealthPoints > 0.6f * mMaxHealthPoints && mHealthPoints <= 0.7f * mMaxHealthPoints) {
        for (int id : {187}) {
            Ground *g = mGame->GetGroundById(id);
            DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
            if (dynamicGround) {
                dynamicGround->SetIsDecreasing(true);
            }
        }
        mMothState = State::FlyUp;
        // mGame->GetCamera()->ChangeCameraMode(CameraMode::ScrollUp);
    }

}


void Moth::TriggerBossDefeat() {
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

void Moth::ManageAnimations() {

}

void Moth::ChangeResolution(float oldScale, float newScale) {

}




