//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawGroundSpritesComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawGroundSpritesComponent::DrawGroundSpritesComponent(Actor* owner, int width, int height, const int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mWidth(width)
    ,mHeight(height)
{
}

void DrawGroundSpritesComponent::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible) {
        return;
    }

    SDL_Texture* texture = mOwner->GetGame()->GetTileSheet();
    std::unordered_map<int, SDL_Rect> tileSheetData = mOwner->GetGame()->GetTileSheetData();

    int numRows = mTilesIndex.size();
    int numCols = mTilesIndex[0].size();

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numCols; col++) {
            SDL_Rect srcRect = tileSheetData[mTilesIndex[row][col]];
            SDL_Rect dstRect;
            dstRect.h = std::ceil(mOwner->GetHeight() / numRows + 1);
            dstRect.w = std::ceil(mOwner->GetWidth() / numCols + 1);
            dstRect.x = mOwner->GetPosition().x - mOwner->GetWidth() / 2 - GetGame()->GetCamera()->GetPosCamera().x + col * mOwner->GetGame()->GetTileSize();
            dstRect.y = mOwner->GetPosition().y - mOwner->GetHeight() / 2 - GetGame()->GetCamera()->GetPosCamera().y + row * mOwner->GetGame()->GetTileSize();

            SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, 0.0f, nullptr, SDL_FLIP_NONE);
        }
    }

    // for (const auto& pair: mSpriteOffsetMap) {
    //     int tileIndex = pair.first;
    //     const std::vector<Vector2>& offsets = pair.second;
    //
    //     SDL_Rect srcRect = tileSheetData[tileIndex];
    //
    //     for (const Vector2& offset: offsets) {
    //         SDL_Rect region;
    //         region.h = mHeight;
    //         region.w = mWidth;
    //         region.x = mOwner->GetPosition().x - GetGame()->GetCamera()->GetPosCamera().x + offset.x;
    //         region.y = mOwner->GetPosition().y - GetGame()->GetCamera()->GetPosCamera().y + offset.y;
    //
    //         SDL_RendererFlip flip = SDL_FLIP_NONE;
    //         if (GetOwner()->GetRotation() == Math::Pi) {
    //             flip = SDL_FLIP_HORIZONTAL;
    //         }
    //
    //         SDL_RenderCopyEx(renderer, texture, &srcRect, &region, 0.0f, nullptr, flip);
    //     }
    // }
}
