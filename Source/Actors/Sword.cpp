//
// Created by roger on 26/04/2025.
//

#include "Sword.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Sword::Sword(class Game *game, Actor *owner, float width, float height, float duration, float damage)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mDuration(duration)
    ,mDurationTimer(mDuration)
    ,mDamage(damage)
    ,mOwner(owner)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawSpriteComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    const std::string swordAssets = "../Assets/Sprites/Sword Slash/";
    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {37, 218, 255, 255});
    // mDrawSpriteComponent = new DrawSpriteComponent(this, swordAssets + "4.png", mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight,
                                                       swordAssets + "SwordSlash.png",
                                                       swordAssets + "SwordSlash.json", 1001);

    const std::vector slash = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9, 9, 9, 9};
    mDrawAnimatedComponent->AddAnimation("slash", slash);

    const std::vector end = {9};
    mDrawAnimatedComponent->AddAnimation("end", end);

    mDrawAnimatedComponent->SetAnimation("end");
    const float fps = 9.0f / mDuration;
    mDrawAnimatedComponent->SetAnimFPS(fps);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void Sword::OnUpdate(float deltaTime) {
    mDurationTimer += deltaTime;
    if (mDurationTimer >= mDuration) {
        Deactivate();
    }
    else {
        Activate();
        // Troca rotação da espada para horizontal ou vertical
        bool isHorizontal = true;
        if (GetRotation() == 0 || GetRotation() == Math::Pi) {
            isHorizontal = true;
        }

        if (GetRotation() == Math::Pi / 2 || GetRotation() == 3 * Math::Pi / 2) {
            isHorizontal = false;
        }

        float offset = 0;
        if (isHorizontal) {
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

            if (mDrawSpriteComponent) {
                mDrawSpriteComponent->SetWidth(mWidth);
                mDrawSpriteComponent->SetHeight(mHeight);
            }
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->SetWidth(mWidth);
                mDrawAnimatedComponent->SetHeight(mHeight);
                mDrawAnimatedComponent->UseRotation(false);
            }
            offset = mWidth * 0.35;
        }
        else {
            Vector2 v1(-mHeight / 2, -mWidth / 2);
            Vector2 v2(mHeight / 2, -mWidth / 2);
            Vector2 v3(mHeight / 2, mWidth / 2);
            Vector2 v4(-mHeight / 2, mWidth / 2);
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

            if (mDrawSpriteComponent) {
                mDrawSpriteComponent->SetWidth(mHeight);
                mDrawSpriteComponent->SetHeight(mWidth);
            }
            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->SetWidth(mWidth);
                mDrawAnimatedComponent->SetHeight(mHeight);
                mDrawAnimatedComponent->UseRotation(true);
            }
            offset = mWidth * 0.35;
        }
        SetPosition(Vector2(mOwner->GetPosition() + GetForward() * offset));
    }
}

void Sword::Activate()
{
    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(true);
    }

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(true);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(true);
        mDrawAnimatedComponent->SetAnimation("slash");
    }
}

void Sword::Deactivate()
{
    SetState(ActorState::Paused);
    mAABBComponent->SetActive(false); // desativa colisão
    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetIsVisible(false);
    }

    if (mDrawSpriteComponent) {
        mDrawSpriteComponent->SetIsVisible(false);
    }

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(false);
        mDrawAnimatedComponent->SetAnimation("end");
    }
    mDurationTimer = 0;
}

void Sword::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

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
