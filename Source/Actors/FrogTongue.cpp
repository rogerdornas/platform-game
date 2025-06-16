//
// Created by roger on 03/06/2025.
//

#include "FrogTongue.h"
#include "Frog.h"
#include "Sword.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawSpriteComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"

FrogTongue::FrogTongue(class Game* game, Frog* owner, float damage)
    :Actor(game)
    ,mWidth(0.0f * mGame->GetScale())
    ,mHeight(44.0f * mGame->GetScale())
    ,mDuration(1.4f)
    ,mDamage(damage)
    ,mOwner(owner)
    ,mNumOfFrames(10)
    ,mFrameDuration(mDuration / 2 / mNumOfFrames)
    ,mFrameTimer(0.0f)
    ,mFrameIndex(0)
    ,mTongueRange(1000.0f)
    ,mGrowSpeed(mTongueRange / (mDuration / 2) * mGame->GetScale())
    ,mIsIncreasing(false)
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

    const std::string frogTongueAssets = "../Assets/Sprites/FrogTongue/";
    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {37, 218, 255, 255});
    // mDrawSpriteComponent = new DrawSpriteComponent(this, swordAssets + "4.png", mWidth, mHeight);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth, mHeight,
                                                       frogTongueAssets + "FrogTongue.png",
                                                       frogTongueAssets + "FrogTongue.json", 1001);


    const std::vector extending0 = {0};
    mDrawAnimatedComponent->AddAnimation("extending0", extending0);

    const std::vector extending1 = {1};
    mDrawAnimatedComponent->AddAnimation("extending1", extending1);

    const std::vector extending2 = {2};
    mDrawAnimatedComponent->AddAnimation("extending2", extending2);

    const std::vector extending3 = {3};
    mDrawAnimatedComponent->AddAnimation("extending3", extending3);

    const std::vector extending4 = {4};
    mDrawAnimatedComponent->AddAnimation("extending4", extending4);

    const std::vector extending5 = {5};
    mDrawAnimatedComponent->AddAnimation("extending5", extending5);

    const std::vector extending6 = {6};
    mDrawAnimatedComponent->AddAnimation("extending6", extending6);

    const std::vector extending7 = {7};
    mDrawAnimatedComponent->AddAnimation("extending7", extending7);

    const std::vector extending8 = {8};
    mDrawAnimatedComponent->AddAnimation("extending8", extending8);

    const std::vector extending9 = {9};
    mDrawAnimatedComponent->AddAnimation("extending9", extending9);

    const std::vector extending10 = {10};
    mDrawAnimatedComponent->AddAnimation("extending10", extending10);


    mDrawAnimatedComponent->SetAnimation("extending0");
    mDrawAnimatedComponent->SetAnimFPS(1.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void FrogTongue::SetDuration(float duration) {
    mDuration = duration;
    mFrameDuration = mDuration / 2 / mNumOfFrames;
    mGrowSpeed = mTongueRange / (mDuration / 2) * mGame->GetScale();
}

void FrogTongue::OnUpdate(float deltaTime) {
    ResolveGroundCollision();
    ResolvePlayerCollision();

    if (mFrameIndex < 1 && !mIsIncreasing) {
        Deactivate();
    }
    else {
        Activate();
        if (mIsIncreasing) {
            mWidth += mGrowSpeed * deltaTime;
        }
        else {
            mWidth -= mGrowSpeed * deltaTime;
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
        if (mDrawSpriteComponent) {
            mDrawSpriteComponent->SetWidth(mWidth);
            mDrawSpriteComponent->SetHeight(mHeight);
        }
        if (mDrawAnimatedComponent) {
            mDrawAnimatedComponent->SetWidth(mWidth);
            mDrawAnimatedComponent->SetHeight(mHeight);
            mDrawAnimatedComponent->UseRotation(false);
            mDrawAnimatedComponent->SetAnimation("extending" + std::to_string(mFrameIndex));
        }
        if (GetRotation() == 0) {
            SetPosition(mOwner->GetPosition() + Vector2(mWidth / 2, 0) + Vector2(30 * mGame->GetScale(), 0));
        }
        else if (GetRotation() == Math::Pi) {
            SetPosition(mOwner->GetPosition() - Vector2(mWidth / 2, 0) - Vector2(30 * mGame->GetScale(), 0));
        }

        mFrameTimer += deltaTime;
        if (mFrameTimer >= mFrameDuration) {
            mFrameTimer -= mFrameDuration;
            if (mFrameIndex >= 10) {
                mIsIncreasing = false;
            }

            if (mIsIncreasing) {
                mFrameIndex++;
            }
            else {
                mFrameIndex--;
            }
        }
    }
}

void FrogTongue::ResolveGroundCollision() {
    std::vector<Ground* > grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground* g: grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                mIsIncreasing = false;
                break;
            }
        }
    }
}

void FrogTongue::ResolvePlayerCollision() {
    Player* player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        player->ReceiveHit(mDamage, GetForward());
        mIsIncreasing = false;
    }
    Sword* sword = player->GetSword();
    if (mAABBComponent->Intersect(*sword->GetComponent<AABBComponent>())) {
        mIsIncreasing = false;
    }
}

void FrogTongue::Activate()
{
    mAABBComponent->SetActive(true); // reativa colisão
    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetIsVisible(true);

    if (mDrawSpriteComponent)
        mDrawSpriteComponent->SetIsVisible(true);

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetIsVisible(true);
    }
    mOwner->SetIsLicking(true);
}

void FrogTongue::Deactivate() {
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
    }
    mFrameTimer = 0;
    mFrameIndex = 0;
    mWidth = 0;
    mIsIncreasing = true;
    mOwner->SetIsLicking(false);
}

void FrogTongue::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));
    mGrowSpeed = mGrowSpeed / oldScale * newScale;

    mRigidBodyComponent->SetVelocity(Vector2(mRigidBodyComponent->GetVelocity().x / oldScale * newScale, mRigidBodyComponent->GetVelocity().y / oldScale * newScale));

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
