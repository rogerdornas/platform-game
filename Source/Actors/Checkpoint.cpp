//
// Created by roger on 18/06/2025.
//

#include "Checkpoint.h"
#include "../Game.h"
#include "../Components/AABBComponent.h"
#include "../Components/DrawComponents/DrawPolygonComponent.h"
#include "../UIElements/UIScreen.h"

Checkpoint::Checkpoint(class Game *game, float width, float height)
    :Actor(game)
    ,mWidth(width)
    ,mHeight(height)
    ,mStoreOpened(false)
    ,mDrawPolygonComponent(nullptr)
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

    mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, {160, 32, 240, 255});
    mAABBComponent = new AABBComponent(this, v1, v3);
}


void Checkpoint::OnProcessInput(const Uint8 *keyState, SDL_GameController &controller) {
    if (mAABBComponent->Intersect(*mGame->GetPlayer()->GetComponent<AABBComponent>())) {
        if (keyState[SDL_SCANCODE_SPACE] || SDL_GameControllerGetButton(&controller, SDL_CONTROLLER_BUTTON_Y)) {
            if (!mGame->GetStore()->StoreOpened() && mGame->GetPlayer()->GetIsOnGround()) {
                mGame->GetStore()->OpenStore();
                mStoreOpened = true;
            }
        }
    }
}

void Checkpoint::OnUpdate(float deltaTime) {
    if (!mAABBComponent->Intersect(*mGame->GetPlayer()->GetComponent<AABBComponent>()) && mGame->GetStore()->StoreOpened() && mStoreOpened) {
        mGame->GetStore()->CloseStore();
        mStoreOpened = false;
    }
}


void Checkpoint::ChangeResolution(float oldScale, float newScale) {
    mWidth = mWidth / oldScale * newScale;
    mHeight = mHeight / oldScale * newScale;
    SetPosition(Vector2(GetPosition().x / oldScale * newScale, GetPosition().y / oldScale * newScale));

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




