//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "DrawSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawSpriteComponent::DrawSpriteComponent(class Actor* owner, const std::string &texturePath, const int width,
                                         const int height, const int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mWidth(width)
    ,mHeight(height)
{
    mSpriteSheetSurface = GetGame()->LoadTexture(texturePath);
}

DrawSpriteComponent::~DrawSpriteComponent() {
    if (mSpriteSheetSurface) {
        SDL_DestroyTexture(mSpriteSheetSurface);
        mSpriteSheetSurface = nullptr;
    }
}

void DrawSpriteComponent::Draw(SDL_Renderer* renderer)
{
    if (!mIsVisible) {
        return;
    }

    SDL_Rect dstRect;
    dstRect.h = mHeight;
    dstRect.w = mWidth;
    dstRect.x = mOwner->GetPosition().x - mWidth / 2 - GetGame()->GetCamera()->GetPosCamera().x;
    dstRect.y = mOwner->GetPosition().y - mHeight / 2 - GetGame()->GetCamera()->GetPosCamera().y;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (GetOwner()->GetRotation() == Math::Pi) {
        flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect, 0.0f, nullptr, flip);
}
