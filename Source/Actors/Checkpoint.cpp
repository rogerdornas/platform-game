//
// Created by roger on 18/06/2025.
//

#include "Checkpoint.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../Components/DrawComponents/DrawAnimatedComponent.h"
#include "../UIElements/UIScreen.h"

Checkpoint::Checkpoint(class Game *game, float width, float height, Vector2 position)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mStoreOpened(false)
    ,mStoreMessageOpened(false)
    ,mDrawPolygonComponent(nullptr)
    ,mDrawAnimatedComponent(nullptr)
{
    SetPosition(position);

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {160, 32, 240, 255});
    mAABBComponent = new AABBComponent(this, v1, v3);

    mDrawAnimatedComponent = new DrawAnimatedComponent(this, mWidth * 1.1f, mHeight * 1.1f,
                                            "../Assets/Sprites/CheckPoint/CheckPoint.png", "../Assets/Sprites/CheckPoint/CheckPoint.json", 1);

    std::vector<int> idle = {0, 1, 2, 3, 4, 5};
    mDrawAnimatedComponent->AddAnimation("idle", idle);

    mDrawAnimatedComponent->SetAnimation("idle");
    mDrawAnimatedComponent->SetAnimFPS(10.0f);
}


void Checkpoint::OnProcessInput(const Uint8 *keyState, SDL_GameController &controller) {
    Player* player = mGame->GetPlayer();
    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        if (keyState[SDL_SCANCODE_SPACE] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_Y)) {
            if (mGame->GetStore()->StoreMessageOpened() && player->GetIsOnGround()) {
                mGame->GetStore()->CloseStoreMessage();
                mStoreMessageOpened = false;
            }

            if (!mGame->GetStore()->StoreOpened() && player->GetIsOnGround()) {
                mGame->GetStore()->OpenStore();
                mStoreOpened = true;
            }
        }
    }
}

void Checkpoint::OnUpdate(float deltaTime) {
    Player* player = mGame->GetPlayer();
    if (!mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        if (mGame->GetStore()->StoreOpened() && mStoreOpened) {
            mGame->GetStore()->CloseStore();
            mStoreOpened = false;
        }
    }

    if (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>())) {
        // mGame->SetCheckPointPosition(GetPosition() + Vector2(-mWidth * 0.8, mHeight / 2));
        mGame->SetCheckPointPosition(GetPosition());
        mGame->SetCheckPointMoney(player->GetMoney());
        player->ResetHealthPoints();
        player->ResetMana();
        player->ResetHealCount();

        if (!mGame->GetStore()->StoreMessageOpened()) {
            mGame->GetStore()->LoadStoreMessage();
            mStoreMessageOpened = true;
        }
    }
    if ((!mAABBComponent->Intersect(*player->GetComponent<AABBComponent>()) &&
        mGame->GetStore()->StoreMessageOpened() && mStoreMessageOpened) ||
        (mAABBComponent->Intersect(*player->GetComponent<AABBComponent>()) &&
        !player->GetIsOnGround() &&
        mGame->GetStore()->StoreMessageOpened() && mStoreMessageOpened))
    {
        mGame->GetStore()->CloseStoreMessage();
        mStoreMessageOpened = false;
    }
}


void Checkpoint::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

    if (mDrawAnimatedComponent) {
        mDrawAnimatedComponent->SetWidth(mWidth * 1.1f);
        mDrawAnimatedComponent->SetHeight(mHeight * 1.1f);
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




