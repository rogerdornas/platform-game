//
// Created by roger on 19/09/2025.
//

#include "Spawner.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

Spawner::Spawner(class Game *game)
    :Actor(game)
    ,mWidth(200 * mGame->GetScale())
    ,mHeight(0)
    ,mMaxHeight(200 * mGame->GetScale())
    ,mSpawnerState(SpawnerState::Appear)
    ,mAppearDuration(0.6f)
    ,mAppearTimer(0.0f)
    ,mOpenDuration(1.2f)
    ,mOpenTimer(0.0f)
    ,mDisappearDuration(0.4f)
    ,mDisappearTimer(0.0f)
    ,mDuration(2.0f)
    ,mTimer(0.0f)
    ,mRigidBodyComponent(nullptr)
    ,mAABBComponent(nullptr)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
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

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000 * mGame->GetScale(), 1600 * mGame->GetScale());
    mAABBComponent = new AABBComponent(this, v1, v3);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, SDL_Color{255, 255, 0, 255}, 997);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.8f, mHeight * 1.8f,
                                            "../Assets/Sprites/Portal2/Portal.png", "../Assets/Sprites/Portal2/Portal.json", 98);

    // std::vector appear = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10};
    // mDrawAnimatedComponent->AddAnimation("appear", appear);

    // std::vector appear = {10};
    // mDrawAnimatedComponent->AddAnimation("appear", appear);
    //
    // std::vector open = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 21};
    // mDrawAnimatedComponent->AddAnimation("open", open);

    // std::vector disappear = {21, 22, 23, 24, 25, 26, 27, 28, 30, 29, 0, 0, 0};
    // mDrawAnimatedComponent->AddAnimation("disappear", disappear);

    // std::vector disappear = {21};
    // mDrawAnimatedComponent->AddAnimation("disappear", disappear);

    std::vector appear = {0, 1, 2, 3, 4, 5, 6};
    mDrawAnimatedComponent->AddAnimation("appear", appear);

    std::vector open = {0, 1, 2, 3, 4, 5, };
    mDrawAnimatedComponent->AddAnimation("open", open);

    std::vector disappear = {0, 1, 2, 3, 4, 5, };
    mDrawAnimatedComponent->AddAnimation("disappear", disappear);


    mDrawAnimatedComponent->SetAnimation("appear");
    mDrawAnimatedComponent->SetAnimFPS(11.0f / mAppearDuration);
}

void Spawner::OnUpdate(float deltaTime) {
    Vector2 v1;
    Vector2 v2;
    Vector2 v3;
    Vector2 v4;
    std::vector<Vector2> vertices;

    switch (mSpawnerState) {
        case SpawnerState::Appear:
            mAppearTimer += deltaTime;
            if (mAppearTimer >= mAppearDuration) {
                mSpawnerState = SpawnerState::Open;
                mDrawAnimatedComponent->SetAnimation("open");
                mDrawAnimatedComponent->SetAnimFPS(13.0f / mOpenDuration);
                mDrawAnimatedComponent->ResetAnimationTimer();
            }

            mHeight = (mAppearTimer / mAppearDuration) * mMaxHeight;

            v1 = Vector2(-mWidth / 2, -mHeight / 2);
            v2 = Vector2(mWidth / 2, -mHeight / 2);
            v3 = Vector2(mWidth / 2, mHeight / 2);
            v4 = Vector2(-mWidth / 2, mHeight / 2);

            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);

            if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
                aabb->SetMin(v1);
                aabb->SetMax(v3);
            }

            if (mDrawPolygonComponent) {
                mDrawPolygonComponent->SetVertices(vertices);
            }

            if (mDrawAnimatedComponent) {
                mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
            }
            break;

        case SpawnerState::Open:
            mOpenTimer += deltaTime;
            if (mOpenTimer >= mOpenDuration) {
                mSpawnerState = SpawnerState::Disappear;
                mDrawAnimatedComponent->SetAnimation("disappear");
                mDrawAnimatedComponent->SetAnimFPS(10.0f / mDisappearDuration);
                mDrawAnimatedComponent->ResetAnimationTimer();
            }
            break;

        case SpawnerState::Disappear:
            mDisappearTimer += deltaTime;
            if (mDisappearTimer >= mDisappearDuration) {
                SetState(ActorState::Destroy);
            }

            mHeight = (1 - (mDisappearTimer / mDisappearDuration)) * mMaxHeight;

            v1 = Vector2(-mWidth / 2, -mHeight / 2);
            v2 = Vector2(mWidth / 2, -mHeight / 2);
            v3 = Vector2(mWidth / 2, mHeight / 2);
            v4 = Vector2(-mWidth / 2, mHeight / 2);

            vertices.emplace_back(v1);
            vertices.emplace_back(v2);
            vertices.emplace_back(v3);
            vertices.emplace_back(v4);

            if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
                aabb->SetMin(v1);
                aabb->SetMax(v3);
            }

            if (mDrawPolygonComponent) {
                mDrawPolygonComponent->SetVertices(vertices);
            }

            if (mDrawAnimatedComponent) {
                    mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
                    mDrawAnimatedComponent->SetHeight(mHeight * 1.8f);
            }
            break;
    }


    // mTimer += deltaTime;
    // if (mTimer >= mDuration) {
    //     SetState(ActorState::Destroy);
    // }
    //
    // GroundCollision
    std::vector<Ground*> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g : grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                mAABBComponent->ResolveCollision(*g->GetComponent<ColliderComponent>());
            }
        }
    }
}

void Spawner::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    mMaxHeight = mMaxHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.8f);
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

    if (mDrawPolygonComponent) {
        mDrawPolygonComponent->SetVertices(vertices);
    }
}


