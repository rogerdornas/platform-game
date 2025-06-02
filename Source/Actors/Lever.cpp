//
// Created by roger on 28/05/2025.
//

#include "Lever.h"

#include "Effect.h"
#include "../Actors/Sword.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../Actors/Ground.h"
#include "../Actors/DynamicGround.h"

Lever::Lever(class Game *game)
    :Trigger(game, 64.0f * game->GetScale(), 64.0f * game->GetScale())
    ,mActivate(false)
    ,mActivatingDuration(0.4f)
    ,mActivatingTimer(0.0f)

    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)

{
    mDrawPolygonComponent = nullptr;

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

void Lever::OnUpdate(float deltaTime) {
    if (!mActivate) {
        Player* player = mGame->GetPlayer();
        Sword* playerSword = player->GetSword();
        if (mAABBComponent->Intersect(*playerSword->GetComponent<AABBComponent>())) {
            mActivate = true;
            // Spark effect
            for (int i = 0; i < 3; i++) {
                auto* sparkEffect = new Effect(mGame);
                sparkEffect->SetDuration(0.1f);
                sparkEffect->SetPosition(GetPosition());
                sparkEffect->SetEffect(TargetEffect::swordHit);
            }
            switch (mTarget) {
                case Target::camera:
                    CameraTrigger();
                break;
                case Target::dynamicGround:
                    DynamicGroundTrigger();
                break;
                case Target::ground:
                    GroundTrigger();
                break;
                default:
                break;
            }
        }
        std::vector<FireBall *> fireBalls = mGame->GetFireBalls();
        for (FireBall *f: fireBalls) {
            if (mAABBComponent->Intersect(*f->GetComponent<AABBComponent>())) {
                mActivate = true;
                switch (mTarget) {
                    case Target::camera:
                        CameraTrigger();
                    break;
                    case Target::dynamicGround:
                        DynamicGroundTrigger();
                    break;
                    case Target::ground:
                        GroundTrigger();
                    break;
                    default:
                    break;
                }
                f->Deactivate();
            }
            break;
        }
    }
    // Ativa animação
    if (mActivate) {
        if (mActivatingTimer < mActivatingDuration) {
            mDrawAnimatedComponent->SetAnimation("activating");
            mActivatingTimer += deltaTime;
        }
        else {
            mDrawAnimatedComponent->SetAnimation("on");
        }
    }
}

void Lever::DynamicGroundTrigger() {
    std::vector<Ground *> grounds = mGame->GetGrounds();
    switch (mEvent) {
        case Event::setIsGrowing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsGrowing(true);
                }
            }
        break;
        case Event::setIsDecreasing:
            for (int id : mGroundsIds) {
                Ground *g = mGame->GetGroundById(id);
                DynamicGround* dynamicGround = dynamic_cast<DynamicGround*>(g);
                if (dynamicGround) {
                    dynamicGround->SetIsDecreasing(true);
                }
            }
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

    mDrawAnimatedComponent->SetWidth(mWidth);
    mDrawAnimatedComponent->SetHeight(mHeight);

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
