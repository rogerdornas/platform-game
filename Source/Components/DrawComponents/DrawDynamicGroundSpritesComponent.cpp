//
// Created by roger on 27/05/2025.
//

#include "DrawDynamicGroundSpritesComponent.h"
#include "../AABBComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Actors/DynamicGround.h"

DrawDynamicGroundSpritesComponent::DrawDynamicGroundSpritesComponent(Actor* owner, int width, int height, const int drawOrder)
    :DrawGroundSpritesComponent(owner, width, height, drawOrder)
{
    mOwnerDynamicGround = dynamic_cast<DynamicGround*>(mOwner);
}

void DrawDynamicGroundSpritesComponent::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible) {
        return;
    }

    float ownerMaxWidth = mOwnerDynamicGround->GetMaxWidth();
    float ownerMaxHeight = mOwnerDynamicGround->GetMaxHeight();
    float ownerWidth = mOwner->GetWidth();
    float ownerHeight = mOwner->GetHeight();
    Vector2 ownerMaxSizeMinPos = mOwnerDynamicGround->GetStartingPosition() - Vector2(ownerMaxWidth, ownerMaxHeight) * 0.5f - GetGame()->GetCamera()->GetPosCamera();
    Vector2 ownerMaxSizeMaxPos = mOwnerDynamicGround->GetStartingPosition() + Vector2(ownerMaxWidth, ownerMaxHeight) * 0.5f - GetGame()->GetCamera()->GetPosCamera();
    Vector2 ownerMinPos = mOwner->GetPosition() - Vector2(ownerWidth, ownerHeight) * 0.5f - GetGame()->GetCamera()->GetPosCamera();
    Vector2 ownerMaxPos = mOwner->GetPosition() + Vector2(ownerWidth, ownerHeight) * 0.5f - GetGame()->GetCamera()->GetPosCamera();


    if (ownerWidth != 0 && ownerHeight != 0) {
        SDL_Texture* texture = mOwner->GetGame()->GetTileSheet();
        std::unordered_map<int, SDL_Rect> tileSheetData = mOwner->GetGame()->GetTileSheetData();

        int numRows = mTilesIndex.size();
        int numCols = mTilesIndex[0].size();
        float tileSize = mOwner->GetGame()->GetTileSize();
        int originalTileSize = mOwner->GetGame()->GetOriginalTileSize();

        for (int row = 0; row < numRows; row++) {
            for (int col = 0; col < numCols; col++) {
                SDL_Rect srcRect = tileSheetData[mTilesIndex[row][col]];
                SDL_Rect dstRect;
                Vector2 tileMinPos = ownerMaxSizeMinPos + Vector2(col, row) * tileSize;
                Vector2 tileMaxPos = ownerMaxSizeMinPos + Vector2(col + 1, row + 1) * tileSize;

                if (tileMinPos.x >= ownerMinPos.x && tileMaxPos.x <= ownerMaxPos.x) {
                    dstRect.x = tileMinPos.x;
                    dstRect.w = std::ceil(tileSize);
                    srcRect.w = originalTileSize;
                }
                else if (tileMinPos.x < ownerMinPos.x && tileMaxPos.x <= ownerMaxPos.x) {
                    dstRect.x = ownerMinPos.x;
                    dstRect.w = std::ceil(tileMaxPos.x - ownerMinPos.x);
                    // srcRect.w = tileMaxPos.x - ownerMinPos.x;
                    srcRect.w = tileMinPos.x + originalTileSize - ownerMinPos.x;
                }
                else if (tileMinPos.x >= ownerMinPos.x && tileMaxPos.x > ownerMaxPos.x) {
                    dstRect.x = tileMinPos.x;
                    dstRect.w = std::ceil(ownerMaxPos.x - tileMinPos.x);
                    srcRect.w = ownerMaxPos.x - tileMinPos.x;
                }

                if (tileMinPos.y >= ownerMinPos.y && tileMaxPos.y <= ownerMaxPos.y) {
                    dstRect.y = tileMinPos.y;
                    dstRect.h = std::ceil(tileSize);
                    srcRect.h = originalTileSize;
                }
                else if (tileMinPos.y < ownerMinPos.y && tileMaxPos.y <= ownerMaxPos.y) {
                    dstRect.y = ownerMinPos.y;
                    dstRect.h = std::ceil(tileMaxPos.y - ownerMinPos.y);
                    // srcRect.h = tileMaxPos.y - ownerMinPos.y;
                    srcRect.h = tileMinPos.y + originalTileSize - ownerMinPos.y;
                }
                else if (tileMinPos.y >= ownerMinPos.y && tileMaxPos.y > ownerMaxPos.y) {
                    dstRect.y = tileMinPos.y;
                    dstRect.h = std::ceil(ownerMaxPos.y - tileMinPos.y);
                    srcRect.h = ownerMaxPos.y - tileMinPos.y;
                }

                SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0.0f, nullptr, SDL_FLIP_NONE);
            }
        }
    }



    // SDL_Texture* texture = mOwner->GetGame()->GetTileSheet();
    // std::unordered_map<int, SDL_Rect> tileSheetData = mOwner->GetGame()->GetTileSheetData();
    //
    // for (const auto &pair: mSpriteOffsetMap) {
    //     int tileIndex = pair.first;
    //     const std::vector<Vector2> &offsets = pair.second;
    //
    //     SDL_Rect srcRect = tileSheetData[tileIndex];
    //
    //     for (const Vector2 &offset: offsets)
    //     {
    //         // owner width
    //         float ownerWidth = mOwner->GetWidth();
    //         // owner height
    //         float ownerHeight = mOwner->GetHeight();
    //
    //         if (ownerWidth != 0 && ownerHeight != 0) {
    //             SDL_Rect region;
    //             // offset pos
    //             Vector2 offsetPos = offset + mOwnerDynamicGround->GetStartingPosition() - GetGame()->GetCamera()->GetPosCamera();
    //
    //             Vector2 ownerMinOffsetPos;
    //             Vector2 ownerMaxOffsetPos;
    //             if (auto* aabb = dynamic_cast<AABBComponent*>(mOwner->GetComponent<ColliderComponent>())) {
    //                 ownerMinOffsetPos = aabb->GetMin() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();
    //                 ownerMaxOffsetPos = aabb->GetMax() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();
    //             }
    //             // Vector2 ownerMinOffsetPos = mOwner->GetComponent<AABBComponent>()->GetMin() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();
    //             // Vector2 ownerMaxOffsetPos = mOwner->GetComponent<AABBComponent>()->GetMax() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();
    //
    //             // Horizontal
    //             if (offsetPos.x >= ownerMinOffsetPos.x && offsetPos.x + mWidth <= ownerMaxOffsetPos.x) {
    //                 region.x = offsetPos.x;
    //                 region.w = mWidth + 1;
    //                 srcRect.w = 32 + 1;
    //             }
    //             else if (offsetPos.x < ownerMinOffsetPos.x && offsetPos.x + mWidth <= ownerMaxOffsetPos.x) {
    //                 region.x = ownerMinOffsetPos.x;
    //                 region.w = offsetPos.x + mWidth - ownerMinOffsetPos.x + 1;
    //                 srcRect.w = offsetPos.x + 32 - ownerMinOffsetPos.x + 1;
    //             }
    //             else if (offsetPos.x >= ownerMinOffsetPos.x && offsetPos.x + mWidth > ownerMaxOffsetPos.x) {
    //                 region.x = offsetPos.x;
    //                 region.w = ownerMaxOffsetPos.x - offsetPos.x + 1;
    //                 srcRect.w = ownerMaxOffsetPos.x - offsetPos.x + 1;
    //             }
    //
    //             // Vertical
    //             if (offsetPos.y >= ownerMinOffsetPos.y && offsetPos.y + mHeight <= ownerMaxOffsetPos.y) {
    //                 region.y = offsetPos.y;
    //                 region.h = mHeight + 1;
    //                 srcRect.h = 32 + 1;
    //             }
    //             else if (offsetPos.y < ownerMinOffsetPos.y && offsetPos.y + mHeight <= ownerMaxOffsetPos.y) {
    //                 region.y = ownerMinOffsetPos.y;
    //                 region.h = offsetPos.y + mHeight - ownerMinOffsetPos.y + 1;
    //                 srcRect.h = offsetPos.y + 32 - ownerMinOffsetPos.y + 1;
    //             }
    //             else if (offsetPos.y >= ownerMinOffsetPos.y && offsetPos.y + mHeight > ownerMaxOffsetPos.y) {
    //                 region.y = offsetPos.y;
    //                 region.h = ownerMaxOffsetPos.y - offsetPos.y + 1;
    //                 srcRect.h = ownerMaxOffsetPos.y - offsetPos.y + 1;;
    //             }
    //
    //             SDL_RendererFlip flip = SDL_FLIP_NONE;
    //             if (GetOwner()->GetRotation() == Math::Pi)
    //                 flip = SDL_FLIP_HORIZONTAL;
    //
    //             SDL_RenderCopyEx(renderer, texture, &srcRect, &region, 0.0f, nullptr, flip);
    //         }
    //     }
    // }
}
