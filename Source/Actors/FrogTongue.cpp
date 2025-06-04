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

FrogTongue::FrogTongue(class Game *game, Actor *owner, float damage)
    :Actor(game)
    ,mWidth(0.0f * mGame->GetScale())
    ,mHeight(44.0f * mGame->GetScale())
    ,mDuration(0.8f)
    ,mDurationTimer(mDuration)
    ,mIncreaseDuration(mDuration / 2)
    ,mIncreaseTimer(mIncreaseDuration)
    ,mDecreaseDuration(mDuration / 2)
    ,mDecreaseTimer(mDecreaseDuration)
    ,mDamage(damage)
    ,mOwner(owner)
    ,mNumOfFrames(10)
    ,mFrameDuration(mDuration / 2 / mNumOfFrames)
    ,mFrameTimer(0.0f)
    ,mFrameIndex(0)
    ,mGrowDist(70.0f * mGame->GetScale())
    ,mGrowSpeed(700 / mIncreaseDuration * mGame->GetScale())
    ,mMaxAttack(1)
    ,mAttackCount(0)
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

    const std::vector increase = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    mDrawAnimatedComponent->AddAnimation("increase", increase);

    const std::vector decrease = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    mDrawAnimatedComponent->AddAnimation("decrease", decrease);

    const std::vector end = {0};
    mDrawAnimatedComponent->AddAnimation("end", end);

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


    // const std::vector extending = {0, 1, 2, 3, 4, 4, 4, 4, 4};
    // mDrawAnimatedComponent->AddAnimation("extending", extending);
    //
    // const std::vector Retracting = {4, 3, 2, 1};
    // mDrawAnimatedComponent->AddAnimation("Retracting", Retracting);

    mDrawAnimatedComponent->SetAnimation("end");
    const float fps = 10.0f / mIncreaseDuration;
    mDrawAnimatedComponent->SetAnimFPS(fps);


    mRigidBodyComponent = new RigidBodyComponent(this, 1, 40000, 1800);
    mAABBComponent = new AABBComponent(this, v1, v3);
}

void FrogTongue::SetDuration(float duration) {
    mDuration = duration;
    mFrameDuration = mDuration / 2 / mNumOfFrames;
    mGrowSpeed = 700 / (mDuration / 2) * mGame->GetScale();
}


void FrogTongue::OnUpdate(float deltaTime) {
    // mDurationTimer += deltaTime;
    // if (mDurationTimer >= mDuration) {
    //     Deactivate();
    //     return;
    // }

    ResolveGroundCollision();
    ResolvePlayerCollision();

    if (mFrameIndex < 1 && !mIsIncreasing) {
        Deactivate();
        return;
    }
    else {
        Activate();
        if (mIsIncreasing) {
            mWidth += mGrowSpeed * deltaTime;
            // mFrameIndex++;
        }
        else {
            mWidth -= mGrowSpeed * deltaTime;
            // mFrameIndex--;
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
                // mFrameTimer += deltaTime;
            }

            if (mIsIncreasing) {
                // mWidth += mGrowSpeed * deltaTime;
                mFrameIndex++;
            }
            else {
                // mWidth -= mGrowSpeed * deltaTime;
                mFrameIndex--;
            }

            // Vector2 v1(-mWidth / 2, -mHeight / 2);
            // Vector2 v2(mWidth / 2, -mHeight / 2);
            // Vector2 v3(mWidth / 2, mHeight / 2);
            // Vector2 v4(-mWidth / 2, mHeight / 2);
            // std::vector<Vector2> vertices;
            // vertices.emplace_back(v1);
            // vertices.emplace_back(v2);
            // vertices.emplace_back(v3);
            // vertices.emplace_back(v4);
            //
            // mAABBComponent->SetMin(v1);
            // mAABBComponent->SetMax(v3);
            //
            // if (mDrawPolygonComponent) {
            //     mDrawPolygonComponent->SetVertices(vertices);
            // }
            // if (mDrawSpriteComponent) {
            //     mDrawSpriteComponent->SetWidth(mWidth);
            //     mDrawSpriteComponent->SetHeight(mHeight);
            // }
            // if (mDrawAnimatedComponent) {
            //     mDrawAnimatedComponent->SetWidth(mWidth);
            //     mDrawAnimatedComponent->SetHeight(mHeight);
            //     mDrawAnimatedComponent->UseRotation(false);
            //     mDrawAnimatedComponent->SetAnimation("extending" + std::to_string(mFrameIndex));
            // }
            // if (GetRotation() == 0) {
            //     SetPosition(mOwner->GetPosition() + Vector2(mWidth / 2, 0) + Vector2(30, 0));
            // }
            // else if (GetRotation() == Math::Pi) {
            //     SetPosition(mOwner->GetPosition() - Vector2(mWidth / 2, 0) - Vector2(30, 0));
            // }
        }















        // if (mIncreaseTimer < mIncreaseDuration) {
        //     mIncreaseTimer += deltaTime;
        //     mDrawAnimatedComponent->SetAnimation("increase");
        //     // mWidth += mGrowDist;
        //     mWidth += mGrowSpeed * deltaTime;
        //
        //     Vector2 v1(-mWidth / 2, -mHeight / 2);
        //     Vector2 v2(mWidth / 2, -mHeight / 2);
        //     Vector2 v3(mWidth / 2, mHeight / 2);
        //     Vector2 v4(-mWidth / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent) {
        //         mDrawPolygonComponent->SetVertices(vertices);
        //     }
        //     if (mDrawSpriteComponent) {
        //         mDrawSpriteComponent->SetWidth(mWidth);
        //         mDrawSpriteComponent->SetHeight(mHeight);
        //     }
        //     if (mDrawAnimatedComponent) {
        //         mDrawAnimatedComponent->SetWidth(mWidth);
        //         mDrawAnimatedComponent->SetHeight(mHeight);
        //         mDrawAnimatedComponent->UseRotation(false);
        //     }
        //     if (GetRotation() == 0) {
        //         SetPosition(mOwner->GetPosition() + Vector2(mWidth / 2, 0) + Vector2(30, 0));
        //     }
        //     else if (GetRotation() == Math::Pi) {
        //         SetPosition(mOwner->GetPosition() - Vector2(mWidth / 2, 0) - Vector2(30, 0));
        //     }
        // }
        // else if (mDecreaseTimer < mDecreaseDuration){
        //     mDecreaseTimer += deltaTime;
        //     mDrawAnimatedComponent->SetAnimation("decrease");
        //     // mWidth -= mGrowDist;
        //     mWidth -= mGrowSpeed * deltaTime;
        //
        //     Vector2 v1(-mWidth / 2, -mHeight / 2);
        //     Vector2 v2(mWidth / 2, -mHeight / 2);
        //     Vector2 v3(mWidth / 2, mHeight / 2);
        //     Vector2 v4(-mWidth / 2, mHeight / 2);
        //     std::vector<Vector2> vertices;
        //     vertices.emplace_back(v1);
        //     vertices.emplace_back(v2);
        //     vertices.emplace_back(v3);
        //     vertices.emplace_back(v4);
        //
        //     mAABBComponent->SetMin(v1);
        //     mAABBComponent->SetMax(v3);
        //
        //     if (mDrawPolygonComponent) {
        //         mDrawPolygonComponent->SetVertices(vertices);
        //     }
        //     if (mDrawSpriteComponent) {
        //         mDrawSpriteComponent->SetWidth(mWidth);
        //         mDrawSpriteComponent->SetHeight(mHeight);
        //     }
        //     if (mDrawAnimatedComponent) {
        //         mDrawAnimatedComponent->SetWidth(mWidth);
        //         mDrawAnimatedComponent->SetHeight(mHeight);
        //         mDrawAnimatedComponent->UseRotation(false);
        //     }
        //     if (GetRotation() == 0) {
        //         SetPosition(mOwner->GetPosition() + Vector2(mWidth / 2, 0) + Vector2(30, 0));
        //     }
        //     else if (GetRotation() == Math::Pi) {
        //         SetPosition(mOwner->GetPosition() - Vector2(mWidth / 2, 0) - Vector2(30, 0));
        //     }
        // }







    //     mFrameTimer += deltaTime;
    //     // SetRotation(mOwner->GetRotation());
    //
    //     if (mFrameTimer >= mFrameDuration) {
    //         if (mFrameIndex >= 10) {
    //             mIsIncreasing = false;
    //         }
    //
    //         // if (mFrameIndex <= 0 && !mIsIncreasing) {
    //         //     Deactivate();
    //         //     return;
    //         // }
    //
    //         if (mIsIncreasing) {
    //             mWidth += mGrowDist;
    //             mFrameIndex++;
    //         }
    //         else {
    //             mWidth -= mGrowDist;
    //             mFrameIndex--;
    //         }
    //
    //         if (mFrameIndex == 0) {
    //             Deactivate();
    //             return;
    //         }
    //
    //         mFrameTimer -= mFrameDuration;
    //
    //         // SDL_Log("%d", mFrameIndex);
    //         Vector2 v1(-mWidth / 2, -mHeight / 2);
    //         Vector2 v2(mWidth / 2, -mHeight / 2);
    //         Vector2 v3(mWidth / 2, mHeight / 2);
    //         Vector2 v4(-mWidth / 2, mHeight / 2);
    //         std::vector<Vector2> vertices;
    //         vertices.emplace_back(v1);
    //         vertices.emplace_back(v2);
    //         vertices.emplace_back(v3);
    //         vertices.emplace_back(v4);
    //
    //         mAABBComponent->SetMin(v1);
    //         mAABBComponent->SetMax(v3);
    //
    //         if (mDrawPolygonComponent) {
    //             mDrawPolygonComponent->SetVertices(vertices);
    //         }
    //         if (mDrawSpriteComponent) {
    //             mDrawSpriteComponent->SetWidth(mWidth);
    //             mDrawSpriteComponent->SetHeight(mHeight);
    //         }
    //         if (mDrawAnimatedComponent) {
    //             mDrawAnimatedComponent->SetWidth(mWidth);
    //             mDrawAnimatedComponent->SetHeight(mHeight);
    //             mDrawAnimatedComponent->SetAnimation("extending" + std::to_string(mFrameIndex));
    //             mDrawAnimatedComponent->UseRotation(false);
    //         }
    //         if (GetRotation() == 0) {
    //             SetPosition(mOwner->GetPosition() + Vector2(mWidth / 2, 0) + Vector2(30, 0));
    //         }
    //         else if (GetRotation() == Math::Pi) {
    //             SetPosition(mOwner->GetPosition() - Vector2(mWidth / 2, 0) - Vector2(mGrowDist, 0));
    //         }
    //     }
    }
}

void FrogTongue::ResolveGroundCollision() {
    std::vector<Ground *> grounds = GetGame()->GetGrounds();
    if (!grounds.empty()) {
        for (Ground *g: grounds) {
            if (mAABBComponent->Intersect(*g->GetComponent<AABBComponent>())) {
                mIsIncreasing = false;
                break;
            }
        }
    }
}

void FrogTongue::ResolvePlayerCollision() {
    Player *player = GetGame()->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        player->ReceiveHit(mDamage, GetForward());
        mIsIncreasing = false;
    }
    Sword *sword = player->GetSword();
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
        // mDrawAnimatedComponent->SetAnimation("extending");
        // mDrawAnimatedComponent->SetAnimation("increase");
    }
    Frog* frog = dynamic_cast<Frog*>(mOwner);
    frog->SetIsLicking(true);
}

void FrogTongue::Deactivate()
{
    // SDL_Log("pausou");
    SetState(ActorState::Paused);
    mAABBComponent->SetActive(false); // desativa colisão
    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetIsVisible(false);

    if (mDrawSpriteComponent)
        mDrawSpriteComponent->SetIsVisible(false);

    if (mDrawAnimatedComponent)
    {
        mDrawAnimatedComponent->SetIsVisible(false);
        // mDrawAnimatedComponent->SetAnimation("end");
    }
    mDurationTimer = 0;
    mIncreaseTimer = 0;
    mDecreaseTimer = 0;
    mFrameTimer = 0;
    mFrameIndex = 0;
    mWidth = 0;
    mIsIncreasing = true;
    Frog* frog = dynamic_cast<Frog*>(mOwner);
    frog->SetIsLicking(false);
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

    if (mDrawPolygonComponent)
        mDrawPolygonComponent->SetVertices(vertices);
}
