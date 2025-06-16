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

    for (const auto& pair: mSpriteOffsetMap) {
        int tileIndex = pair.first;
        const std::vector<Vector2>& offsets = pair.second;

        SDL_Rect srcRect = tileSheetData[tileIndex];

        for (const Vector2& offset: offsets) {
            SDL_Rect region;
            region.h = mHeight;
            region.w = mWidth;
            region.x = mOwner->GetPosition().x - GetGame()->GetCamera()->GetPosCamera().x + offset.x;
            region.y = mOwner->GetPosition().y - GetGame()->GetCamera()->GetPosCamera().y + offset.y;

            SDL_RendererFlip flip = SDL_FLIP_NONE;
            if (GetOwner()->GetRotation() == Math::Pi) {
                flip = SDL_FLIP_HORIZONTAL;
            }

            SDL_RenderCopyEx(renderer, texture, &srcRect, &region, 0.0f, nullptr, flip);
        }
    }
}
