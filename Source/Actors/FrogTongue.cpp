//
// Created by roger on 03/06/2025.
//

#include "FrogTongue.h"
#include "Frog.h"
#include "Sword.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"

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
    ,mDrawComponent(nullptr)
    ,mRectComponent(nullptr)
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

    mDrawComponent = new AnimatorComponent(this, frogTongueAssets + "FrogTongue.png",
                                                       frogTongueAssets + "FrogTongue.json",
                                                       mWidth, mHeight, 1001);


    const std::vector extending0 = {0};
    mDrawComponent->AddAnimation("extending0", extending0);

    const std::vector extending1 = {1};
    mDrawComponent->AddAnimation("extending1", extending1);

    const std::vector extending2 = {2};
    mDrawComponent->AddAnimation("extending2", extending2);

    const std::vector extending3 = {3};
    mDrawComponent->AddAnimation("extending3", extending3);

    const std::vector extending4 = {4};
    mDrawComponent->AddAnimation("extending4", extending4);

    const std::vector extending5 = {5};
    mDrawComponent->AddAnimation("extending5", extending5);

    const std::vector extending6 = {6};
    mDrawComponent->AddAnimation("extending6", extending6);

    const std::vector extending7 = {7};
    mDrawComponent->AddAnimation("extending7", extending7);

    const std::vector extending8 = {8};
    mDrawComponent->AddAnimation("extending8", extending8);

    const std::vector extending9 = {9};
    mDrawComponent->AddAnimation("extending9", extending9);

    const std::vector extending10 = {10};
    mDrawComponent->AddAnimation("extending10", extending10);


    mDrawComponent->SetAnimation("extending0");
    mDrawComponent->SetAnimFPS(1.0f);

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

        if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
            aabb->SetMin(v1);
            aabb->SetMax(v3);
        }

        if (mRectComponent) {
            // mDrawPolygonComponent->SetVertices(vertices);
            mRectComponent->SetWidth(mWidth);
            mRectComponent->SetHeight(mHeight);
        }

        if (mDrawComponent) {
            mDrawComponent->SetWidth(mWidth);
            mDrawComponent->SetHeight(mHeight);
            // mDrawAnimatedComponent->UseRotation(false);
            mDrawComponent->SetAnimation("extending" + std::to_string(mFrameIndex));
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
            if (mAABBComponent->Intersect(*g->GetComponent<ColliderComponent>())) {
                mIsIncreasing = false;
                break;
            }
        }
    }
}

void FrogTongue::ResolvePlayerCollision() {
    Player* player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<ColliderComponent>())) {
        player->ReceiveHit(mDamage, GetForward());
        mIsIncreasing = false;
    }
    Sword* sword = player->GetSword();
    if (mAABBComponent->Intersect(*sword->GetComponent<ColliderComponent>())) {
        mIsIncreasing = false;
    }
}

void FrogTongue::Activate()
{
    mAABBComponent->SetActive(true); // reativa colisão
    if (mRectComponent) {
        mRectComponent->SetVisible(true);
    }

    if (mDrawComponent) {
        mDrawComponent->SetVisible(true);
    }
    mOwner->SetIsLicking(true);
}

void FrogTongue::Deactivate() {
    SetState(ActorState::Paused);
    mAABBComponent->SetActive(false); // desativa colisão
    if (mRectComponent) {
        mRectComponent->SetVisible(false);
    }

    if (mDrawComponent) {
        mDrawComponent->SetVisible(false);
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

    // if (mDrawAnimatedComponent) {
    //     mDrawAnimatedComponent->SetWidth(mWidth);
    //     mDrawAnimatedComponent->SetHeight(mHeight);
    // }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }

    // if (mDrawPolygonComponent) {
    //     mDrawPolygonComponent->SetVertices(vertices);
    // }
}
