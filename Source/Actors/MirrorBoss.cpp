//
// Created by roger on 11/10/2025.
//

#include "MirrorBoss.h"
#include "CloneEnemy.h"
#include "EnemySimple.h"
#include "FlyingEnemySimple.h"
#include "FlyingShooterEnemy.h"
#include "Lever.h"
#include "Mushroom.h"
#include "Projectile.h"
#include "Skill.h"
#include "Snake.h"
#include "Spawner.h"
#include "../Game.h"
#include "../HUD.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Random.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Actors/DynamicGround.h"

MirrorBoss::MirrorBoss(Game *game)
    :Enemy(game)
    ,mBossState(State::Stop)

    ,mStopDuration(1.5f)
    ,mStopDurationFast(0.2f)
    ,mStopDurationSlow(1.5f)
    ,mStopTimer(0.0f)

    ,mHitDuration(0.3f)

    ,mProjectileWidth(80 * mGame->GetScale())
    ,mProjectileHeight(80 * mGame->GetScale())
    ,mProjectileSpeed(1000 * mGame->GetScale())
    ,mProjectileDamage(10)
    ,mProjectileDuration(8.0f)
    ,mProjectileSpread(Math::Pi / 2)
    ,mNumProjectileBounces(7)
    ,mMaxProjectiles(6)
    ,mCountProjectiles(0)
    ,mDurationBetweenProjectiles(0.25f)
    ,mTimerBetweenProjectiles(0.0f)

    ,mMirrorWidth(100 * mGame->GetScale())
    ,mMirrorHeight(200 * mGame->GetScale())

    ,mSpawnEnemyDuration(6.0f)
    ,mSpawnEnemyTimer(0.0f)
    ,mAlreadySpawnedEnemy(false)
    ,mSpawnPortalDuration(1.7f)
    ,mSpawnPortalTimer(0.0f)
    ,mCloneEnemy(nullptr)
    ,mCloneEnemyDied(false)

    ,mNumSpawnEnemies(3)

    ,mTeleportDuration(0.6f)
    ,mTeleportDurationFast(0.2f)
    ,mTeleportDurationNormal(0.6f)
    ,mTeleportInTimer(0.0f)
    ,mTeleportOutTimer(0.0f)
    ,mDistFromPlayerToStartTeleport(550.0f * mGame->GetScale())
    ,mDistFromPlayerToEndTeleport(650.0f * mGame->GetScale())
{
    mWidth = 120 * mGame->GetScale();
    mHeight = 250 * mGame->GetScale();
    mMoveSpeed = 500 * mGame->GetScale();
    mHealthPoints = 700;
    mMaxHealthPoints = mHealthPoints;
    mContactDamage = 20;
    mMoneyDrop = 200;
    mKnockBackSpeed = 0.0f * mGame->GetScale();
    mKnockBackDuration = 0.0f;
    mKnockBackTimer = mKnockBackDuration;
    mEnemyCollision = false;

    SetSize(mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mHeight * 1.8f * 0.667f, mHeight * 1.8f, "../Assets/Sprites/MirrorBoss/MirrorBoss.png", "../Assets/Sprites/MirrorBoss/MirrorBoss.json", 998);
    std::vector idle = {0};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(1.0f);

    // mSpawnPoints = mGame->GetSpawnPointsPositions();
}

void MirrorBoss::OnUpdate(float deltaTime) {
    mKnockBackTimer += deltaTime;

    if (mFlashTimer < mHitDuration) {
        if (mFlashTimer == 0)
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

    if (mCloneEnemy && mCloneEnemy->GetState() == ActorState::Destroy && mAlreadySpawnedEnemy) {
        mSpawnPortalTimer = 0;
        mSpawnEnemyTimer = 0;
        mAlreadySpawnedEnemy = false;
        mCloneEnemyDied = false;
        mBossState = State::Stop;
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

    if (mDrawAnimatedComponent) {
        ManageAnimations();
    }
}

void MirrorBoss::MovementBeforePlayerSpotted() {

}

void MirrorBoss::MovementAfterPlayerSpotted(float deltaTime) {
    if (mGame->GetPlayer()->GetState() != ActorState::Active) {
        mBossState = State::Stop;
    }
    switch (mBossState) {
        case State::Stop:
            Stop(deltaTime);
        break;

        case State::Projectiles:
            Projectiles(deltaTime);
        break;

        case State::InvertControl:
            InvertControl(deltaTime);
        break;

        case State::SpawnEnemy:
            SpawnEnemy(deltaTime);
        break;

        case State::SpawnCloneEnemy:
            SpawnCloneEnemy(deltaTime);
        break;

        case State::TeleportIn:
            TeleportIn(deltaTime);
        break;

        case State::TeleportOut:
            TeleportOut(deltaTime);
        break;
    }
}

void MirrorBoss::Stop(float deltaTime) {
    mStopTimer += deltaTime;

    if (mAlreadySpawnedEnemy) {
        mStopDuration = mStopDurationFast;
    }
    else {
        mStopDuration = mStopDurationSlow;
    }

    if (mStopTimer >= mStopDuration) {
        mStopTimer = 0;

        // if (!mGame->GetPlayer()->GetIsInvertControls()) {
        //     mBossState = State::InvertControl;
        // }

        // mBossState = State::SpawnCloneEnemy;

        // mBossState = State::TeleportIn;

        // mBossState = State::Projectiles;

        if (mHealthPoints >= mMaxHealthPoints * 0.7f) {
            if (Random::GetFloat() < 0.4) {
                mBossState = State::Projectiles;
            }
            else if (Random::GetFloat() < 0.25) {
                mBossState = State::InvertControl;
            }
            else {
                mBossState = State::TeleportIn;
            }
        }

        if (mHealthPoints >= mMaxHealthPoints * 0.45f && mHealthPoints < mMaxHealthPoints * 0.7f) {
            if (mAlreadySpawnedEnemy) {
                float dist = (mGame->GetPlayer()->GetPosition() - GetPosition()).Length();
                std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
                for (FireBall* f: fireBalls) {
                    if (f->GetState() == ActorState::Active) {
                        float fireBallDist = (f->GetPosition() - GetPosition()).Length();
                        dist = std::min(dist, fireBallDist);
                    }
                }

                if (dist <= mDistFromPlayerToStartTeleport) {
                    mBossState = State::TeleportIn;
                }
            }
            else if (Random::GetFloat() < 0.25) {
                mBossState = State::Projectiles;
            }
            else if (Random::GetFloat() < 0.25) {
                mBossState = State::TeleportIn;
            }
            else if (Random::GetFloat() < 0.4) {
                mBossState = State::SpawnCloneEnemy;
            }
            else {
                mBossState = State::InvertControl;
            }
        }
        if (mHealthPoints < mMaxHealthPoints * 0.45f) {
            if (mAlreadySpawnedEnemy) {
                float dist = (mGame->GetPlayer()->GetPosition() - GetPosition()).Length();
                std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
                for (FireBall* f: fireBalls) {
                    if (f->GetState() == ActorState::Active) {
                        float fireBallDist = (f->GetPosition() - GetPosition()).Length();
                        dist = std::min(dist, fireBallDist);
                    }
                }

                if (dist <= mDistFromPlayerToStartTeleport) {
                    mBossState = State::TeleportIn;
                }
                else {
                    if (Random::GetFloat() < 0.1) {
                        mBossState = State::Projectiles;
                    }
                }
            }
            else if (Random::GetFloat() < 0.25) {
                mBossState = State::Projectiles;
            }
            else if (Random::GetFloat() < 0.25) {
                mBossState = State::TeleportIn;
            }
            else if (Random::GetFloat() < 0.4) {
                mBossState = State::SpawnCloneEnemy;
            }
            else {
                mBossState = State::InvertControl;
            }
        }
    }

    // Player* player = GetGame()->GetPlayer();
    // float dist = GetPosition().x - player->GetPosition().x;
    // if (dist < 0) {
    //     SetRotation(0.0);
    // }
    // else {
    //     SetRotation(Math::Pi);
    // }

    if (mKnockBackTimer >= mKnockBackDuration) {
        mRigidBodyComponent->SetVelocity(Vector2(0, 0));
    }
}

void MirrorBoss::Projectiles(float deltaTime) {
    mTimerBetweenProjectiles += deltaTime;

    if (mCountProjectiles >= mMaxProjectiles) {
        mCountProjectiles = 0;
        mBossState = State::Stop;
        return;
    }

    if (mTimerBetweenProjectiles >= mDurationBetweenProjectiles) {
        mTimerBetweenProjectiles -= mDurationBetweenProjectiles;

        Vector2 toPlayer = mGame->GetPlayer()->GetPosition() - GetPosition();
        float direction = atan2f(toPlayer.y, toPlayer.x);

        float randomOffset = Random::GetFloatRange(-mProjectileSpread, mProjectileSpread);

        direction += randomOffset;

        // float direction = Math::Pi * (static_cast<float>(mCountProjectiles) / mMaxProjectiles);

        std::vector<Projectile* > projectiles = mGame->GetProjectiles();
        for (Projectile* p: projectiles) {
            if (p->GetState() == ActorState::Paused && p->GetProjectileType() == Projectile::ProjectileType::OrangeBall) {
                p->SetState(ActorState::Active);
                p->SetRotation(direction);
                p->SetWidth(mProjectileWidth);
                p->SetHeight(mProjectileHeight);
                p->SetSpeed(mProjectileSpeed);
                p->SetDuration(mProjectileDuration);
                p->SetDamage(mProjectileDamage);
                p->SetMaxBounce(mNumProjectileBounces);
                p->SetPosition(GetPosition());
                p->GetComponent<DrawAnimatedComponent>()->UseRotation(true);
                break;
            }
        }
        mCountProjectiles++;
    }
}

void MirrorBoss::InvertControl(float deltaTime) {
    if (mGame->GetPlayer()->GetIsInvertControls()) {
        mBossState = State::Stop;
        return;
    }

    mGame->GetPlayer()->SetInvertControls(true);

    mSpawnPoints = mGame->GetSpawnPointsPositions();

    int index = Random::GetIntRange(0, mSpawnPoints.size() - 1);
    Vector2 position = mSpawnPoints[index];

    while (position == GetPosition()) {
        index = Random::GetIntRange(0, mSpawnPoints.size() - 1);
        position = mSpawnPoints[index];
    }

    auto* mirror = new Lever(mGame, mMirrorWidth, mMirrorHeight, Lever::LeverType::Mirror);
    mirror->SetPosition(position);
    mirror->SetTarget("Player");
    mirror->SetEvent("RevertControls");
    mBossState = State::Stop;
}

void MirrorBoss::SpawnEnemy(float deltaTime) {
    if (mSpawnPortalTimer == 0) {
        mSpawnPoints = mGame->GetSpawnPointsPositions();
        mSpawnIndexes.clear();

        while (mSpawnIndexes.size() < mNumSpawnEnemies) {
            mSpawnIndexes.insert(Random::GetIntRange(0, mSpawnPoints.size() - 1));
        }
        for (int index : mSpawnIndexes) {
            Vector2 position = mSpawnPoints[index];
            auto* spawner = new Spawner(mGame);
            spawner->SetPosition(position);
        }
    }

    mSpawnPortalTimer += deltaTime;

    if (mSpawnPortalTimer >= mSpawnPortalDuration) {
        // Coloca todos os inimigos possíveis em um vetor
        std::vector<Enemies> allEnemies = {
            Enemies::Snake,
            Enemies::FlyingEnemySimple,
            Enemies::FlyingShooterEnemy,
            Enemies::EnemySimple
        };

        // Embaralha a ordem
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(allEnemies.begin(), allEnemies.end(), gen);

        // Pega os 3 primeiros (distintos e aleatórios)
        std::vector<Enemies> chosen(allEnemies.begin(), std::next(allEnemies.begin(), 3));

        int i = 0;
        for (int index : mSpawnIndexes) {
            Vector2 position = mSpawnPoints[index];
            if (chosen[i] == Enemies::Snake) {
                auto* snake = new Snake(mGame);
                snake->SetPosition(position);
                snake->SetSpottedPlayer(true);
            }
            if (chosen[i] == Enemies::FlyingEnemySimple) {
                auto* flyingEnemySimple = new FlyingEnemySimple(mGame);
                flyingEnemySimple->SetPosition(position);
                flyingEnemySimple->SetSpottedPlayer(true);
            }
            if (chosen[i] == Enemies::FlyingShooterEnemy) {
                auto* flyingShooterEnemy = new FlyingShooterEnemy(mGame);
                flyingShooterEnemy->SetPosition(position);
                flyingShooterEnemy->SetSpottedPlayer(true);
            }
            if (chosen[i] == Enemies::EnemySimple) {
                auto* enemySimple = new EnemySimple(mGame);
                enemySimple->SetPosition(position);
                enemySimple->SetSpottedPlayer(true);
            }
            i++;
        }
        mSpawnPortalTimer = 0;
        mBossState = State::Stop;
        return;
    }
}

void MirrorBoss::SpawnCloneEnemy(float deltaTime) {
    if (mAlreadySpawnedEnemy) {
        if (mHealthPoints < mMaxHealthPoints * 0.4f) {
            mBossState = State::SpawnEnemy;
        }
        else {
            mBossState = State::Stop;
        }
        return;
    }

    if (mSpawnEnemyTimer == 0) {
        mSpawnPosition = Vector2(Random::GetFloatRange(mArenaMinPos.x, mArenaMaxPos.x), mArenaMaxPos.y);
        auto* spawner = new Spawner(mGame);
        spawner->SetPosition(mSpawnPosition);
    }
    mSpawnPortalTimer += deltaTime;
    mSpawnEnemyTimer += deltaTime;
    // if (mSpawnEnemyTimer >= mSpawnEnemyDuration) {
    //     mSpawnPortalTimer = 0;
    //     mSpawnEnemyTimer = 0;
    //     mAlreadySpawnedEnemy = false;
    //     mBossState = State::Stop;
    // }

    if (mSpawnPortalTimer >= mSpawnPortalDuration && !mAlreadySpawnedEnemy) {
        mCloneEnemy = new CloneEnemy(mGame);
        mCloneEnemy->SetPosition(mSpawnPosition);
        mCloneEnemy->SetSpottedPlayer(true);
        mAlreadySpawnedEnemy = true;
        mCloneEnemyDied = false;
        mSpawnPortalTimer = 0;
        if (mHealthPoints < mMaxHealthPoints * 0.4f) {
            mBossState = State::SpawnEnemy;
        }
        else {
            mBossState = State::Stop;
        }
        return;
    }

    if (mCloneEnemy && mCloneEnemy->GetState() == ActorState::Destroy && mAlreadySpawnedEnemy) {
        mCloneEnemyDied = true;
    }

    if (mCloneEnemyDied) {
        mSpawnPortalTimer = 0;
        mSpawnEnemyTimer = 0;
        mAlreadySpawnedEnemy = false;
        mCloneEnemyDied = false;
        if (mHealthPoints < mMaxHealthPoints * 0.4f) {
            mBossState = State::SpawnEnemy;
        }
        else {
            mBossState = State::Stop;
        }
    }
}

void MirrorBoss::TeleportIn(float deltaTime) {
    mTeleportInTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));

    if (mAlreadySpawnedEnemy) {
        mTeleportDuration = mTeleportDurationFast;
    }
    else {
        mTeleportDuration = mTeleportDurationNormal;
    }

    if (mTeleportInTimer >= mTeleportDuration) {
        // mTeleportTargetPosition.x = Random::GetFloatRange(mArenaMinPos.x + mWidth, mArenaMaxPos.x - mWidth);
        // mTeleportTargetPosition.y = Random::GetFloatRange(mArenaMinPos.y + mHeight, mArenaMaxPos.y - mHeight);

        mSpawnPoints = mGame->GetSpawnPointsPositions();

        int index = Random::GetIntRange(0, mSpawnPoints.size() - 1);
        mTeleportTargetPosition = mSpawnPoints[index];

        while ((mTeleportTargetPosition - mGame->GetPlayer()->GetPosition()).Length() < mDistFromPlayerToEndTeleport) {
            index = Random::GetIntRange(0, mSpawnPoints.size() - 1);
            mTeleportTargetPosition = mSpawnPoints[index];
        }

        SetPosition(mTeleportTargetPosition);

        mTeleportInTimer = 0;
        mBossState = State::TeleportOut;
        if (mDrawAnimatedComponent) {
            mDrawAnimatedComponent->ResetAnimationTimer();
        }
        return;
    }

    if (mTeleportInTimer > 0.3f * mTeleportDuration) {
        mColliderComponent->SetActive(false);
    }
}

void MirrorBoss::TeleportOut(float deltaTime) {
    mTeleportOutTimer += deltaTime;
    mRigidBodyComponent->SetVelocity(Vector2(0, 0));

    if (mTeleportOutTimer >= mTeleportDuration) {
        mTeleportOutTimer = 0;
        mBossState = State::Stop;
        return;
    }

    if (mTeleportOutTimer > 0.1f * mTeleportDuration) {
        mColliderComponent->SetActive(true);
    }
}

void MirrorBoss::TriggerBossDefeat() {
    if (mCloneEnemy && mAlreadySpawnedEnemy) {
        mCloneEnemy->SetState(ActorState::Destroy);
    }

    mGame->SetWorldFlag("MirrorBossDefeated", true);
    mGame->StopBossMusic();
}

void MirrorBoss::ManageAnimations() {
    mDrawAnimatedComponent->SetTransparency(255);

    if (mBossState == State::TeleportIn) {
        mDrawAnimatedComponent->SetTransparency(static_cast<int>((1.0f - mTeleportInTimer / mTeleportDuration) * 255));
    }
    else if (mBossState == State::TeleportOut) {
        mDrawAnimatedComponent->SetTransparency(static_cast<int>(mTeleportOutTimer / mTeleportDuration * 255));
    }
}

void MirrorBoss::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMoveSpeed = mMoveSpeed / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mKnockBackSpeed = mKnockBackSpeed / oldScale * newScale;
    mCameraShakeStrength = mCameraShakeStrength / oldScale * newScale;
    mProjectileWidth = mProjectileWidth / oldScale * newScale;
    mProjectileHeight = mProjectileHeight / oldScale * newScale;
    mProjectileSpeed = mProjectileSpeed / oldScale * newScale;
    mMirrorWidth = mMirrorWidth / oldScale * newScale;
    mMirrorHeight = mMirrorHeight / oldScale * newScale;
    mDistFromPlayerToStartTeleport = mDistFromPlayerToStartTeleport / oldScale * newScale;
    mDistFromPlayerToEndTeleport = mDistFromPlayerToEndTeleport / oldScale * newScale;
    mArenaMinPos.x = mArenaMinPos.x / oldScale * newScale;
    mArenaMinPos.y = mArenaMinPos.y / oldScale * newScale;
    mArenaMaxPos.x = mArenaMaxPos.x / oldScale * newScale;
    mArenaMaxPos.y = mArenaMaxPos.y / oldScale * newScale;
    mSpawnPosition.x = mSpawnPosition.x / oldScale * newScale;
    mSpawnPosition.y = mSpawnPosition.y / oldScale * newScale;

    for (Vector2& pos : mSpawnPoints) {
        pos.x = pos.x / oldScale * newScale;
        pos.y = pos.y / oldScale * newScale;
    }

    mTeleportTargetPosition.x = mTeleportTargetPosition.x / oldScale * newScale;
    mTeleportTargetPosition.y = mTeleportTargetPosition.y / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mHeight * 1.8f * 0.667f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
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
