#include "UIScreen.h"
#include "../Game.h"
#include "UIFont.h"
#include <cfloat>

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mIsVisible(true)
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
}

void UIScreen::Update(float deltaTime) {

}

void UIScreen::Draw(Renderer *renderer)
{
    if (!mIsVisible) {
        return;
    }

    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }
}

void UIScreen::ProcessInput(const uint8_t* keys)
{

}

void UIScreen::HandleKeyPress(int key, int controllerButton, int controllerAxisY, int controllerAxisX)
{
    if (mButtons.empty()) {
        return;
    }

    UIButton* current = mButtons[mSelectedButtonIndex];
    UIButton* next = nullptr;
    auto inputBinding = mGame->GetInputBinding();

    if (key == SDLK_UP ||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Up].key) ||
        controllerButton == SDL_CONTROLLER_BUTTON_DPAD_UP ||
        controllerAxisY < 0)
    {
        next = FindNeighbor(current, Vector2(0, -1));
    }
    else if (key == SDLK_DOWN ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Down].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_DOWN ||
             controllerAxisY > 0)
    {
        next = FindNeighbor(current, Vector2(0, 1));
    }
    else if (key == SDLK_LEFT ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveLeft].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_LEFT ||
             controllerAxisX < 0)
    {
        next = FindNeighbor(current, Vector2(-1, 0));
    }
    else if (key == SDLK_RIGHT ||
             key == SDL_GetKeyFromScancode(inputBinding[Game::Action::MoveRight].key) ||
             controllerButton == SDL_CONTROLLER_BUTTON_DPAD_RIGHT ||
             controllerAxisX > 0)
    {
        next = FindNeighbor(current, Vector2(1, 0));
    }

    // Troca seleção se houver vizinho
    if (next) {
        mSelectedButtonIndex = static_cast<int>(
            std::distance(mButtons.begin(),
                          std::find(mButtons.begin(), mButtons.end(), next))
        );
    }

    // Ativa botão selecionado
    if (key == SDLK_RETURN ||
        controllerButton == SDL_CONTROLLER_BUTTON_A||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Jump].key) ||
        key == SDL_GetKeyFromScancode(inputBinding[Game::Action::Attack].key))
    {
        if (mSelectedButtonIndex >= 0 && mSelectedButtonIndex < (int)mButtons.size()) {
            mButtons[mSelectedButtonIndex]->OnClick();
            if (mGame->GetPlayer()) {
                mGame->GetPlayer()->SetCanJump(false);
                mGame->GetPlayer()->SetPrevSwordPressed(true);
            }
        }
    }

    // Atualiza destaque de todos os botões
    for (size_t i = 0; i < mButtons.size(); ++i) {
        mButtons[i]->SetHighlighted(static_cast<int>(i) == mSelectedButtonIndex);
    }
}

void UIScreen::HandleMouse(const SDL_Event &event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            int x = event.button.x;
            int y = event.button.y;
            for (UIButton* button : mButtons) {
                if (button->ContainsPoint(Vector2(x, y) - GetPosition())) {
                    button->OnClick();
                }
            }
        }
    }

    if (event.type == SDL_MOUSEMOTION) {
        int x = event.motion.x;
        int y = event.motion.y;

        int index = -1;

        for (size_t i = 0; i < mButtons.size(); ++i) {
            if (mButtons[i]->ContainsPoint(Vector2(x, y) - GetPosition())) {
                index = i;
                mSelectedButtonIndex = i;
            }
        }

        // Atualiza destaque dos botões
        if (index != -1) {
            for (size_t i = 0; i < mButtons.size(); ++i) {
                mButtons[i]->SetHighlighted(static_cast<int>(i) == index);
            }
        }
    }
}

void UIScreen::HandleMousePress(const Vector2& virtualMousePos)
{
    // Coordenadas já estão no espaço virtual,
    // mas a lógica do botão espera coordenadas relativas ao UIScreen
    Vector2 uiScreenRelativePos = virtualMousePos - GetPosition();

    for (UIButton* button : mButtons) {
        if (button->ContainsPoint(uiScreenRelativePos)) {
            button->OnClick();
        }
    }
}

void UIScreen::HandleMouseMotion(const Vector2& virtualMousePos)
{
    Vector2 uiScreenRelativePos = virtualMousePos - GetPosition();
    int index = -1;

    for (size_t i = 0; i < mButtons.size(); ++i) {
        if (mButtons[i]->ContainsPoint(uiScreenRelativePos)) {
            index = i;
            mSelectedButtonIndex = i;
        }
    }

    // Atualiza destaque
    if (index != -1) {
        for (size_t i = 0; i < mButtons.size(); ++i) {
            mButtons[i]->SetHighlighted(static_cast<int>(i) == index);
        }
    }
    // Nota: você pode querer um 'else' aqui para desmarcar
    // se o mouse sair de todos os botões (index == -1)
}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string &name, const Vector2 &pos, const Vector2 &dims, const int pointSize, Vector3 color, const int unsigned wrapLength)
{
    UIText* t = new UIText(name, mFont, pointSize, wrapLength, pos, dims, color);

    mTexts.emplace_back(t);

    return t;
}

UIButton* UIScreen::AddButton(const std::string& name, const Vector2 &pos, const Vector2& dims, const int pointSize, UIButton::TextPos alignText, std::function<void()> onClick, Vector2 textPos, Vector3 textColor)
{
    UIButton* b = new UIButton(name, mFont, onClick, pos, dims, Vector3{1.0f, 0.5f, 0.0f}, pointSize, 1024, textPos, alignText, textColor);
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

// Retorna true se há interseção no eixo perpendicular
bool IntersectsY(UIButton* a, UIButton* b) {
    float topA = a->GetPosition().y;
    float bottomA = topA + a->GetSize().y;
    float topB = b->GetPosition().y;
    float bottomB = topB + b->GetSize().y;
    return !(bottomA <= topB || bottomB <= topA);
}

bool IntersectsX(UIButton* a, UIButton* b) {
    float leftA = a->GetPosition().x;
    float rightA = leftA + a->GetSize().x;
    float leftB = b->GetPosition().x;
    float rightB = leftB + b->GetSize().x;
    return !(rightA <= leftB || rightB <= leftA);
}

UIButton* UIScreen::FindNeighbor(UIButton *current, const Vector2 &dir) {
    UIButton* best = nullptr;
    float bestDist = FLT_MAX;

    // ---- 1) Preferência: botões que intersectam no eixo perpendicular ----
    for (UIButton* b : mButtons) {
        if (b == current) continue;

        Vector2 delta = b->GetPosition() - current->GetPosition();

        // Só considera se está na direção certa
        if ((dir.x > 0 && delta.x <= 0) || (dir.x < 0 && delta.x >= 0) ||
            (dir.y > 0 && delta.y <= 0) || (dir.y < 0 && delta.y >= 0))
        {
            continue;
        }

        bool intersects;
        if (dir.x != 0) {
            intersects = IntersectsY(current, b);
        }
        else {
            intersects = IntersectsX(current, b);
        }

        if (!intersects) continue;

        float dist = delta.LengthSq();
        if (dist < bestDist) {
            bestDist = dist;
            best = b;
        }
    }

    // ---- 2) Se não achou nenhum alinhado, pega o mais próximo "livre" na direção ----
    if (!best) {
        bestDist = FLT_MAX;
        for (UIButton* b : mButtons) {
            if (b == current) continue;

            Vector2 delta = b->GetPosition() - current->GetPosition();

            if ((dir.x > 0 && delta.x <= 0) || (dir.x < 0 && delta.x >= 0) ||
                (dir.y > 0 && delta.y <= 0) || (dir.y < 0 && delta.y >= 0))
            {
                continue;
            }

            float dist = delta.LengthSq();
            if (dist < bestDist) {
                bestDist = dist;
                best = b;
            }
        }
    }

    // ---- 3) Se ainda não achou, aplica wrap no mesmo eixo ----
    if (!best) {
        if (dir.x != 0) {
            // Procurar botão MAIS distante no eixo X
            float extremeX = (dir.x > 0 ? -FLT_MAX : FLT_MAX);

            for (UIButton* b : mButtons) {
                if (b == current) continue;

                if (!IntersectsY(current, b)) continue;

                float x = b->GetPosition().x;
                if (dir.x > 0) { // indo para direita → pega o menor X
                    if (x < extremeX || extremeX == -FLT_MAX) {
                        extremeX = x;
                        best = b;
                    }
                } else { // indo para esquerda → pega o maior X
                    if (x > extremeX || extremeX == FLT_MAX) {
                        extremeX = x;
                        best = b;
                    }
                }
            }
        }
        else if (dir.y != 0) {
            // Procurar botão MAIS distante no eixo Y
            float extremeY = (dir.y > 0 ? -FLT_MAX : FLT_MAX);

            for (UIButton* b : mButtons) {
                if (b == current) continue;

                if (!IntersectsX(current, b)) continue;

                float y = b->GetPosition().y;
                if (dir.y > 0) { // indo para baixo → pega o menor Y
                    if (y < extremeY || extremeY == -FLT_MAX) {
                        extremeY = y;
                        best = b;
                    }
                } else { // indo para cima → pega o maior Y
                    if (y > extremeY || extremeY == FLT_MAX) {
                        extremeY = y;
                        best = b;
                    }
                }
            }
        }
    }

    return best;
}


void UIScreen::ChangeResolution(float oldScale, float newScale) {
    mPos.x = mPos.x / oldScale * newScale;
    mPos.y = mPos.y / oldScale * newScale;
    mSize.x = mSize.x / oldScale * newScale;
    mSize.y = mSize.y / oldScale * newScale;

    for (UIImage* image : mImages) {
        image->ChangeResolution(oldScale, newScale);
    }

    for (UIButton* button: mButtons) {
        button->ChangeResolution(oldScale, newScale);
    }

    for (UIText* text: mTexts) {
        text->ChangeResolution(oldScale, newScale);
    }
}
