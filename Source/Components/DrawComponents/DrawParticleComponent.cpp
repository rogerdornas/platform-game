//
// Created by roger on 29/05/2025.
//

#include "DrawParticleComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

DrawParticleComponent::DrawParticleComponent(class Actor* owner, const std::string& texturePath, const int width,
                                         const int height, SDL_Color color, int drawOrder)
    :DrawSpriteComponent(owner, texturePath, width, height, drawOrder)
    ,mColor(color)
{
}

void DrawParticleComponent::Draw(SDL_Renderer* renderer)
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

    float angle = 0;
    angle = Math::ToDegrees(GetOwner()->GetRotation());

    SDL_SetTextureColorMod(mSpriteSheetSurface, mColor.r,mColor.g, mColor.b);

    // Define blend mode e transparência
    SDL_SetTextureBlendMode(mSpriteSheetSurface, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(mSpriteSheetSurface, mColor.a);

    SDL_RenderCopyEx(renderer, mSpriteSheetSurface, nullptr, &dstRect, angle, nullptr, flip);
}
