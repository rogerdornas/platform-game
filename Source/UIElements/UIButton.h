//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <functional>
#include <string>
#include "UIFont.h"
#include "UIText.h"
#include "UIElement.h"
#include "../Math.h"

class UIButton : public UIElement
{
public:
    enum class TextPos {
        AlignLeft,
        AlignRight,
        Center
    };

    UIButton(const std::string& text, class UIFont* font, std::function<void()> onClick,
             const Vector2& pos, const Vector2& size, const Vector3& color = Vector3(30, 30, 30),
             int pointSize = 72, const unsigned wrapLength = 1024,
             const Vector2& textPos = Vector2::Zero,
             const TextPos textAlign = TextPos::Center,
             const Vector3& textColor = Color::White,
             const Vector2& textSize = Vector2(140.f, 20.0f));

    ~UIButton();

    // Set the name of the button
    void SetText(const std::string& text);
    UIText* GetText() const { return mText; }
    void Draw(SDL_Renderer* renderer, const Vector2 &screenPos) override;

    void SetHighlighted(bool sel) { mHighlighted = sel; }
    bool GetHighlighted() const { return mHighlighted; }

    // Returns true if the point is within the button's bounds
    bool ContainsPoint(const Vector2& pt) const;

    // Called when button is clicked
    void OnClick();

    void ChangeResolution(float oldScale, float newScale) override;

private:
    // Callback function
    std::function<void()> mOnClick;

    // Button name
    UIText* mText;

    // Alinhamento do texto do botão
    TextPos mTextAlign;

    // Check if the button is highlighted
    bool mHighlighted;
};