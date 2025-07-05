//
// Created by Lucas N. Ferreira on 28/05/25.
//

#include "UIImage.h"

UIImage::UIImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &size, const Vector3 &color, SDL_Renderer* renderer)
    :UIElement(pos, size, color)
    ,mTexture(nullptr)
    ,mRenderer(renderer)
{
    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
    }
    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);
}

UIImage::~UIImage()
{
    if (mTexture) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}

void UIImage::SetImage(const std::string& imagePath)
{
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }

    SDL_Surface* surface = IMG_Load(imagePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
    }

    mTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
    SDL_FreeSurface(surface);
}

void UIImage::Draw(SDL_Renderer* renderer, const Vector2 &screenPos)
{
    if (mTexture == nullptr) {
        return;
    }

    SDL_Rect dstRect;
    dstRect.x = mPosition.x + screenPos.x;
    dstRect.y = mPosition.y + screenPos.y;
    dstRect.w = mSize.x;
    dstRect.h = mSize.y;

    SDL_RenderCopyEx(renderer, mTexture, nullptr, &dstRect, 0.0, nullptr, SDL_FLIP_NONE);
}

void UIImage::ChangeResolution(float oldScale, float newScale) {
    mPosition.x = mPosition.x / oldScale * newScale;
    mPosition.y = mPosition.y / oldScale * newScale;
    mSize.x = mSize.x / oldScale * newScale;
    mSize.y = mSize.y / oldScale * newScale;
}
