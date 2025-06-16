#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
{
    mGame->PushUI(this);

    mFont = mGame->LoadFont(fontName);
}

UIScreen::~UIScreen()
{
    for (UIText* text : mTexts) {
        delete text;
    }
    mTexts.clear();

    for (UIButton* button : mButtons) {
        delete button;
    }
    mButtons.clear();

    for (UIImage* image : mImages) {
        delete image;
    }
    mImages.clear();

    mGame->GetUIStack().pop_back();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::Draw(SDL_Renderer *renderer)
{

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key, int controllerButton, int controllerAxisY)
{
    if (key == SDLK_UP ||
        controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP ||
        controllerAxisY < 0) {
        // Move para o botão anterior
        mSelectedButtonIndex--;
        if (mSelectedButtonIndex < 0) {
            mSelectedButtonIndex = static_cast<int>(mButtons.size()) - 1;
        }
    }
    else if (key == SDLK_DOWN ||
        controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
        controllerAxisY > 0) {
        // Move para o próximo botão
        mSelectedButtonIndex++;
        if (mSelectedButtonIndex >= static_cast<int>(mButtons.size())) {
            mSelectedButtonIndex = 0;
        }
    }
    else if (key == SDLK_RETURN || controllerButton == SDL_CONTROLLER_BUTTON_A) {
        // Ativa o botão selecionado
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < static_cast<int>(mButtons.size())) {
            mButtons[mSelectedButtonIndex]->OnClick();
        }
    }

    // Atualiza destaque dos botões
    for (size_t i = 0; i < mButtons.size(); ++i)
    {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
    }
}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, const int unsigned wrapLength)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, Color::White);

    mTexts.emplace_back(t);

    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, const int pointSize, std::function<void()> onClick)
{
    UIButton* b = new UIButton(name, mFont, onClick, pos, dims, Vector3{1.0f, 0.5f, 0.0f}, pointSize, 1024, Vector2::Zero);
    mButtons.emplace_back(b);

    if (mButtons.size() == 1) {
        mSelectedButtonIndex = 0;
        b->SetHighlighted(true);
    }

    return b;
}

UIImage* UIScreen::AddImage(const std::string &imagePath, const Vector2 &pos, const Vector2 &dims, const Vector3 &color)
{
    UIImage* img = new UIImage(imagePath, pos, dims, color, mGame->GetRenderer());

    mImages.emplace_back(img);

    return img;
}

void UIScreen::ChangeResolution(float oldScale, float newScale) {
    mPos.x = mPos.x / oldScale * newScale;
    mPos.y = mPos.y / oldScale * newScale;
    mSize.x = mSize.x / oldScale * newScale;
    mSize.y = mSize.y / oldScale * newScale;

    for (UIButton* button: mButtons) {
        button->ChangeResolution(oldScale, newScale);
    }

    for (UIText* text: mTexts) {
        text->ChangeResolution(oldScale, newScale);
    }
}
