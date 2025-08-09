//
// Created by roger on 28/07/2025.
//

#include "HookPoint.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

HookPoint::HookPoint(class Game *game)
    :Actor(game)
    ,mWidth(48 * mGame->GetScale())
    ,mHeight(48 * mGame->GetScale())
    ,mRadius(530.0f * mGame->GetScale())
    ,mHookPointState(HookPointState::OutRange)

    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
    ,mDrawPolygonComponent(nullptr)
{
    // Componente visual
    Vector2 v1(-mWidth/2, -mHeight/2);
    Vector2 v2(mWidth/2, -mHeight/2);
    Vector2 v3(mWidth/2, mHeight/2);
    Vector2 v4(-mWidth/2, mHeight/2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 5000);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mWidth,
                                               "../Assets/Sprites/HookPoint/HookPoint.png",
                                               "../Assets/Sprites/HookPoint/HookPoint.json", 999);

    std::vector idle = {0};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    std::vector illuminated = {1};
    mDrawAnimatedComponent->AddAnimation("illuminated", illuminated);

    std::vector hooked = {2};
    mDrawAnimatedComponent->AddAnimation("hooked", hooked);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(1.0f);


    mAABBComponent = new AABBComponent(this, v1, v3);

    mGame->AddHookPoint(this);
}

HookPoint::~HookPoint() {
    mGame->RemoveHookPoint(this);
}

void HookPoint::OnUpdate(float deltaTime) {
    switch (mHookPointState) {
        case HookPointState::OutRange:
            mDrawAnimatedComponent->SetAnimation("idle");
            break;
        case HookPointState::InRange:
            mDrawAnimatedComponent->SetAnimation("illuminated");
            break;
        case HookPointState::Hooked:
            mDrawAnimatedComponent->SetAnimation("hooked");
            break;
    }

    mHookPointState = HookPointState::OutRange;
}

void HookPoint::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mRadius = mRadius / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetWidth(mWidth);
        mDrawSpriteComponent->SetHeight(mHeight);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth);
        mDrawAnimatedComponent->SetHeight(mHeight);
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


