//
// Created by roger on 28/05/2025.
//

#include "Lever.h"

#include "Effect.h"
#include "Golem.h"
#include "../Actors/Sword.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Actors/Ground.h"
#include "../Actors/DynamicGround.h"

Lever::Lever(class Game* game, float width, float height, LeverType leverType)
    :Trigger(game, width * game->GetScale(), height * game->GetScale())
    ,mLeverType(leverType)
    ,mSwordHit(false)
    ,mActivate(false)
    ,mActivatingDuration(0.4f)
    ,mActivatingTimer(0.0f)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

{
    mDrawPolygonComponent = nullptr;

    if (mLeverType == LeverType::Lever) {
        mHealthPoints = 1;
        mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight,
                                           "../Assets/Sprites/Lever/Lever.png",
                                           "../Assets/Sprites/Lever/Lever.json", 150);

        std::vector off = {0};
        mDrawAnimatedComponent->AddAnimation("off", off);

        std::vector activating = {0, 1, 2, 3, 4, 5, 6, 6, 6, 6};
        mDrawAnimatedComponent->AddAnimation("activating", activating);

        std::vector on = {6};
        mDrawAnimatedComponent->AddAnimation("on", on);

        mDrawAnimatedComponent->SetAnimation("off");
        const float fps = 7.0f / mActivatingDuration;
        mDrawAnimatedComponent->SetAnimFPS(fps);
    }
    else if (mLeverType == LeverType::Crystal) {
        mHealthPoints = 50;
        mDrawSpriteComponent = new DrawSpriteComponent(this, "../Assets/Sprites/Crystal/Crystal2.png", mWidth, mHeight);
    }
}

void Lever::OnUpdate(float deltaTime) {
    if (!mActivate) {
        Player* player = mGame->GetPlayer();
        Sword* playerSword = player->GetSword();
        if (playerSword->GetState() == ActorState::Paused) {
            mSwordHit = false;
        }

        if (mSwordHit == false) {
            if (mAABBComponent->Intersect(*playerSword->GetComponent<AABBComponent>())) {
                mSwordHit = true;
                if (mLeverType == LeverType::Lever) {
                    mGame->GetAudio()->PlaySound("HitSpike/HitSpike1.wav");
                }
                if (mLeverType == LeverType::Crystal) {
                    mGame->GetAudio()->PlayVariantSound("HitEnemy/HitEnemy.wav", 4);
                }
                mHealthPoints -= playerSword->GetDamage();
                // Spark effect
                for (int i = 0; i < 3; i++) {
                    auto* sparkEffect = new Effect(mGame);
                    sparkEffect->SetDuration(0.1f);
                    sparkEffect->SetPosition(GetPosition());
                    sparkEffect->SetEffect(TargetEffect::SwordHit);
                }
            }
        }

        std::vector<FireBall* > fireBalls = mGame->GetFireBalls();
        for (FireBall* f: fireBalls) {
            if (!f->GetIsFromEnemy())
            if (mAABBComponent->Intersect(*f->GetComponent<AABBComponent>())) {
                mHealthPoints -= f->GetDamage();
                f->ExplodeFireball();
            }
            break;
        }

        if (mHealthPoints <= 0) {
            mActivate = true;
            switch (mTarget) {
                case Target::Camera:
                    CameraTrigger();
                break;

                case Target::DynamicGround:
                    DynamicGroundTrigger();
                break;

                case Target::Ground:
                    GroundTrigger();
                break;

                case Target::Enemy:
                    EnemyTrigger();
                break;

                default:
                    break;
            }

        }
    }
    // Ativa animação
    if (mActivate) {
        if (mLeverType == LeverType::Lever) {
            if (mActivatingTimer < mActivatingDuration) {
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetAnimation("activating");
                }
                mActivatingTimer += deltaTime;
            }
            else {
                if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetAnimation("on");
                }
            }
        }
        if (mLeverType == LeverType::Crystal) {
            if (mDrawSpriteComponent) {
                mDrawSpriteComponent->SetIsVisible(false);
                SetState(ActorState::Destroy);
            }
        }
    }
}

void Lever::DynamicGroundTrigger() {
    std::vector<Ground* > grounds = mGame->GetGrounds();
    switch (mEvent) {
        case Event::SetIsGrowing:
            for (int id : mGroundsIds) {
                Ground* g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsGrowing(true);
                }
            }
        break;
        case Event::SetIsDecreasing:
            for (int id : mGroundsIds) {
                Ground* g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsDecreasing(true);
                }
            }
        default:
            break;
    }
}

void Lever::EnemyTrigger() {
    std::vector<Enemy *> enemies = mGame->GetEnemies();
    switch (mEvent) {
        case Event::SpotPlayer:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e) {
                    e->SetSpottedPlayer(true);
                }
            }
        SetState(ActorState::Destroy);
        break;

        case Event::GolemVulnerable:
            for (int id : mEnemiesIds) {
                Enemy *e = mGame->GetEnemyById(id);
                if (e) {
                    Golem* golem = dynamic_cast<Golem*>(e);
                    golem->SetIsVulnerable();
                }
            }
        break;

        default:
            break;
    }
}



void Lever::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mFixedCameraPosition.x = mFixedCameraPosition.x / oldScale * newScale;
    mFixedCameraPosition.y = mFixedCameraPosition.y / oldScale * newScale;

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth);
        mDrawAnimatedComponent->SetHeight(mHeight);
    }

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth);
        mDrawSpriteComponent->SetHeight(mHeight);
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

    mAABBComponent->SetMin(v1);
    mAABBComponent->SetMax(v3);

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}
