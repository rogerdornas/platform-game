//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"
#include "../Renderer/Renderer.h"

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


void UIButton::Draw(Renderer *renderer, const Vector2 &screenPos)
{
    // Desenhar o retângulo do botão
    Vector2 drawPos = screenPos + mPosition + mSize / 2;

    if (mHighlighted) {
        Vector3 drawColor = Vector3(200, 100, 0);
        renderer->DrawRect(drawPos, mSize, 0.0f, drawColor, Vector2::Zero, RendererMode::TRIANGLES);
    }
    // Calcular posição do texto
    Vector2 textDrawPos = drawPos;
    switch (mTextAlign) {
        case TextPos::AlignLeft:
            textDrawPos += Vector2(10.0f, (mSize.y - mText->GetSize().y) * 0.5f);
        break;
        case TextPos::AlignRight:
            textDrawPos += Vector2(mSize.x - mText->GetSize().x - 10.0f,
                                   (mSize.y - mText->GetSize().y) * 0.5f);
        break;
        case TextPos::Center:
            default:
                textDrawPos += (mSize * 0.5f) - (mText->GetSize() * 0.5f);
        break;
    }

    mText->Draw(renderer, textDrawPos);
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
