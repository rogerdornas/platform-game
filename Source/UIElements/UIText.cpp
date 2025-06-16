//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "UIFont.h"

UIText::UIText(const std::string &text, class UIFont* font, int pointSize, const unsigned wrapLength,
               const Vector2 &pos, const Vector2 &size, const Vector3 &color)
    :UIElement(pos, size, color)
    ,mFont(font)
    ,mPointSize(pointSize)
    ,mWrapLength(wrapLength)
    ,mTextTexture(nullptr)
{
    SetText(text);
}

UIText::~UIText()
{
    if (mTextTexture) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }
}

void UIText::SetText(const std::string &text)
{
    if (mTextTexture != nullptr) {
        SDL_DestroyTexture(mTextTexture);
        mTextTexture = nullptr;
    }
    mTextTexture = mFont->RenderText(text, mColor, mPointSize, mWrapLength);
    mText = text;

    int texWidth, texHeight;
    SDL_QueryTexture(mTextTexture, nullptr, nullptr, &texWidth, &texHeight);
    SetSize(Vector2(texWidth, texHeight));
}

void UIText::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    SDL_Rect titleQuad;
    titleQuad.x = mPosition.x + screenPos.x;
    titleQuad.y = mPosition.y + screenPos.y;
    titleQuad.w = mSize.x;
    titleQuad.h = mSize.y;

    SDL_RenderCopyEx(renderer, mTextTexture, nullptr, &titleQuad, 0.0, nullptr, SDL_FLIP_NONE);
}

void UIText::ChangeResolution(float oldScale, float newScale) {
    mPosition.x = mPosition.x / oldScale * newScale;
    mPosition.y = mPosition.y / oldScale * newScale;
    mSize.x = mSize.x / oldScale * newScale;
    mSize.y = mSize.y / oldScale * newScale;

    mPointSize = mPointSize / oldScale * newScale;
    SetText(mText);
}
