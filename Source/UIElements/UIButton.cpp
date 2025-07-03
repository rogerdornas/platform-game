//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"

UIButton::UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
                    const Vector2& pos, const Vector2& size, const Vector3& color,
                    int pointSize , unsigned wrapLength,
                    const Vector2 &textPos, TextPos textAlign, const Vector3& textColor, const Vector2 &textSize)
        :UIElement(pos, size, color)
        ,mOnClick(onClick)
        ,mHighlighted(false)
        ,mTextAlign(textAlign)
{
    mText = new UIText(text, font, pointSize, wrapLength, textPos, textSize, textColor);
}

UIButton::~UIButton()
{
    delete mText;
    mText = nullptr;
}


void UIButton::Draw(SDL_Renderer *renderer, const Vector2 &screenPos)
{
    SDL_Rect titleQuad = {static_cast<int>(screenPos.x + mPosition.x),
                          static_cast<int>(screenPos.y + mPosition.y),
                          static_cast<int>(mSize.x),
                          static_cast<int>(mSize.y)};

    if (mHighlighted) {
        SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
        SDL_RenderFillRect(renderer, &titleQuad);
    }

    if (mTextAlign == TextPos::AlignLeft) {
        mText->Draw(renderer, screenPos + mPosition);
    }
    else if (mTextAlign == TextPos::Center) {
        mText->Draw(renderer, screenPos + mPosition + mSize * 0.5f - mText->GetSize() * 0.5f);
    }
}

bool UIButton::ContainsPoint(const Vector2 &pt) const {
    if (pt.x < mPosition.x || pt.x > mPosition.x + mSize.x ||
        pt.y < mPosition.y || pt.y > mPosition.y + mSize.y)
    {
        return false;
    }
    return true;
}


void UIButton::OnClick()
{
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::ChangeResolution(float oldScale, float newScale) {
    mPosition.x = mPosition.x / oldScale * newScale;
    mPosition.y = mPosition.y / oldScale * newScale;
    mSize.x = mSize.x / oldScale * newScale;
    mSize.y = mSize.y / oldScale * newScale;

    mText->ChangeResolution(oldScale, newScale);
}
