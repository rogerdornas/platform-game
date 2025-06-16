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

    SDL_Texture* texture = mOwner->GetGame()->GetTileSheet();
    std::unordered_map<int, SDL_Rect> tileSheetData = mOwner->GetGame()->GetTileSheetData();

    for (const auto &pair: mSpriteOffsetMap) {
        int tileIndex = pair.first;
        const std::vector<Vector2> &offsets = pair.second;

        SDL_Rect srcRect = tileSheetData[tileIndex];

        for (const Vector2 &offset: offsets)
        {
            // owner width
            float ownerWidth = mOwner->GetWidth();
            // owner height
            float ownerHeight = mOwner->GetHeight();

            if (ownerWidth != 0 && ownerHeight != 0) {
                SDL_Rect region;
                // offset pos
                Vector2 offsetPos = offset + mOwnerDynamicGround->GetStartingPosition() - GetGame()->GetCamera()->GetPosCamera();

                Vector2 ownerMinOffsetPos = mOwner->GetComponent<AABBComponent>()->GetMin() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();
                Vector2 ownerMaxOffsetPos = mOwner->GetComponent<AABBComponent>()->GetMax() + mOwner->GetPosition() - GetGame()->GetCamera()->GetPosCamera();

                // Horizontal
                if (offsetPos.x >= ownerMinOffsetPos.x && offsetPos.x + mWidth <= ownerMaxOffsetPos.x) {
                    region.x = offsetPos.x;
                    region.w = mWidth + 1;
                    srcRect.w = 32 + 1;
                }
                else if (offsetPos.x < ownerMinOffsetPos.x && offsetPos.x + mWidth <= ownerMaxOffsetPos.x) {
                    region.x = ownerMinOffsetPos.x;
                    region.w = offsetPos.x + mWidth - ownerMinOffsetPos.x + 1;
                    srcRect.w = offsetPos.x + 32 - ownerMinOffsetPos.x + 1;
                }
                else if (offsetPos.x >= ownerMinOffsetPos.x && offsetPos.x + mWidth > ownerMaxOffsetPos.x) {
                    region.x = offsetPos.x;
                    region.w = ownerMaxOffsetPos.x - offsetPos.x + 1;
                    srcRect.w = ownerMaxOffsetPos.x - offsetPos.x + 1;
                }

                // Vertical
                if (offsetPos.y >= ownerMinOffsetPos.y && offsetPos.y + mHeight <= ownerMaxOffsetPos.y) {
                    region.y = offsetPos.y;
                    region.h = mHeight + 1;
                    srcRect.h = 32 + 1;
                }
                else if (offsetPos.y < ownerMinOffsetPos.y && offsetPos.y + mHeight <= ownerMaxOffsetPos.y) {
                    region.y = ownerMinOffsetPos.y;
                    region.h = offsetPos.y + mHeight - ownerMinOffsetPos.y + 1;
                    srcRect.h = offsetPos.y + 32 - ownerMinOffsetPos.y + 1;
                }
                else if (offsetPos.y >= ownerMinOffsetPos.y && offsetPos.y + mHeight > ownerMaxOffsetPos.y) {
                    region.y = offsetPos.y;
                    region.h = ownerMaxOffsetPos.y - offsetPos.y + 1;
                    srcRect.h = ownerMaxOffsetPos.y - offsetPos.y + 1;;
                }

                SDL_RendererFlip flip = SDL_FLIP_NONE;
                if (GetOwner()->GetRotation() == Math::Pi)
                    flip = SDL_FLIP_HORIZONTAL;

                SDL_RenderCopyEx(renderer, texture, &srcRect, &region, 0.0f, nullptr, flip);
            }
        }
    }
}
