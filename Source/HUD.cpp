//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    ,mSpeedHPDecrease(200.0f * mGame->GetScale())
    ,mSpeedHPIncrease(400.0f * mGame->GetScale())
    ,mWaitToDecreaseDuration(0.7f)
    ,mWaitToDecreaseTimer(0.0f)
    ,mWaitToDecreaseManaDuration(0.7f)
    ,mWaitToDecreaseManaTimer(0.0f)
    ,mPlayerDie(false)
{
    float HPBarX = 20 * mGame->GetScale();
    float HPBarY = 20 * mGame->GetScale();
    float HPBarWidth = 400 * mGame->GetScale();
    float HPBarHeight = 30 * mGame->GetScale();

    float ManaBarX = 20 * mGame->GetScale();
    float ManaBarY = 55 * mGame->GetScale();
    float ManaBarWidth = 250 * mGame->GetScale();
    float ManaBarHeight = 30 * mGame->GetScale();

    mHPBar = {HPBarX,HPBarY,HPBarWidth,HPBarHeight};
    mDamageTakenBar = mHPBar;
    mHPRemainingBar = mHPBar;
    mHPGrowingBar = mHPBar;

    mManaBar = {ManaBarX, ManaBarY, ManaBarWidth, ManaBarHeight};
    mManaUsedBar = mManaBar;
    mManaRemainingBar = mManaBar;

    // --------------
    // TODO - PARTE 3
    // --------------
    mPlayerHealCount = AddText(std::to_string(mGame->GetPlayer()->GetHealCount()), Vector2(20, 90) * mGame->GetScale(), Vector2(CHAR_WIDTH, WORD_HEIGHT) * mGame->GetScale(), POINT_SIZE * mGame->GetScale());
    // TODO 1.: Adicione um texto com a string "Time" no canto superior direito da tela, como no jogo orginal. Note que
    //  a classe HUD tem constantes WORD_HEIGHT, WORD_OFFSET, CHAR_WIDTH, POINT_SIZE e HUD_POS_Y que podem ser usadas
    //  para posicionar e definir o tamanho do texto.
    // AddText("Time", Vector2(mGame->GetLogicalWindowWidth() - 100, HUD_POS_Y), Vector2(4 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);

    // TODO 2.: Adicione um texto com a string "400" (400 segundos) logo abaixo do texto "Time".
    //  Guarde o ponteiro do texto em um membro chamado mTimeText.
    // mTimeText = AddText("400", Vector2(mGame->GetLogicalWindowWidth() - 100, HUD_POS_Y + WORD_HEIGHT), Vector2(3 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);

    // TODO 3.: Adicione um texto com a string "World" à esquerda do texto "Time", como no jogo original.
    // AddText("World", Vector2(mGame->GetLogicalWindowWidth() - 250, HUD_POS_Y), Vector2(5 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);

    // TODO 4.: Adicione um texto com a string "1-1" logo abaixo do texto "World".
    // mLevelName = AddText("1-1", Vector2(mGame->GetLogicalWindowWidth() - 250, HUD_POS_Y + WORD_HEIGHT), Vector2(3 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);

    // TODO 5.: Adicione um texto com a string "Mario" no canto superior esquerdo da tela, como no jogo original.
    // AddText("Mario", Vector2(50, HUD_POS_Y), Vector2(5 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);

    // TODO 6.: Adicione um texto com a string "000000" logo abaixo do texto "Mario".
    // mScoreCounter = AddText("000000", Vector2(50, HUD_POS_Y + WORD_HEIGHT), Vector2(6 * CHAR_WIDTH, WORD_HEIGHT), POINT_SIZE);
}

HUD::~HUD()
{

}

void HUD::Update(float deltaTime) {
    if (!mPlayerDie) {
        float playerHealthPoints = mGame->GetPlayer()->GetHealthPoints() / mGame->GetPlayer()->GetMaxHealthPoints();
        if (playerHealthPoints < 0) {
            playerHealthPoints = 0;
        }
        mHPRemainingBar.w = mHPBar.w * playerHealthPoints;

        if (mHPGrowingBar.w < mHPRemainingBar.w) {
            mHPGrowingBar.w += mSpeedHPIncrease * deltaTime;
            if (mHPGrowingBar.w > mHPRemainingBar.w) {
                mHPGrowingBar.w = mHPRemainingBar.w;
            }
        }
        else {
            mHPGrowingBar.w = mHPRemainingBar.w;
        }

        float playerMana = mGame->GetPlayer()->GetMana() / mGame->GetPlayer()->GetMaxMana();
        if (playerMana < 0) {
            playerMana = 0;
        }
        mManaRemainingBar.w = mManaBar.w * playerMana;

        std::string playerHealCount = std::to_string(mGame->GetPlayer()->GetHealCount());
        mPlayerHealCount->SetText(playerHealCount);
    }

    if (mDamageTakenBar.w > mHPGrowingBar.w) {
        mWaitToDecreaseTimer += deltaTime;
        if (mWaitToDecreaseTimer >= mWaitToDecreaseDuration) {
            mDamageTakenBar.w -= mSpeedHPDecrease * deltaTime;
        }
    }
    else {
        mDamageTakenBar.w = mHPGrowingBar.w;
        mWaitToDecreaseTimer = 0;
    }

    if (mManaUsedBar.w > mManaRemainingBar.w) {
        mWaitToDecreaseManaTimer += deltaTime;
        if (mWaitToDecreaseManaTimer >= mWaitToDecreaseManaDuration) {
            mManaUsedBar.w -= mSpeedHPDecrease * deltaTime;
        }
    }
    else {
        mManaUsedBar.w = mManaRemainingBar.w;
        mWaitToDecreaseManaTimer = 0;
    }


    if (mGame->GetPlayer()->Died()) {
        mPlayerDie = true;
    }
}


void HUD::SetTime(int time)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mTimeText para atualizar o texto com o tempo restante. Lembre-se que
    //  o tempo é um inteiro que representa os segundos restantes, e deve ser convertido para string.
    std::string timeStr = std::to_string(time);
    mTimeText->SetText(timeStr);

    // TODO 2.: A posição e o tamanho do texto irão mudar dependendo do número de dígitos na variável time.
    //  Ajuste a posição e o tamanho do mTimeText de acordo com o número de dígitos, de tal forma que
    //  o texto fique alinhado à direita com o texto "Time" e o tamanho do texto seja proporcional ao número de dígitos.
    int numDigits = timeStr.length();

    float alignRightX = mGame->GetLogicalWindowWidth() - 100 + 4 * CHAR_WIDTH; // posição x da borda direita do texto "Time"
    mTimeText->SetPosition(Vector2(alignRightX - numDigits * CHAR_WIDTH, HUD_POS_Y + WORD_HEIGHT));
}

void HUD::SetLevelName(const std::string &levelName)
{
    // --------------
    // TODO - PARTE 3
    // --------------

    // TODO 1.: Utilize o método SetText() do mLevelName para atualizar o texto com o nome do nível.
    mLevelName->SetText(levelName);
}

void HUD::Draw(class SDL_Renderer *renderer) {
    // --------------
    // TODO - PARTE 1-1
    // --------------

    // TODO 1.: Percorra a listas de textos (mTexts) e chame o método Draw de cada UIText, passando o renderer
    //  e a posição da tela (mPos).
    for (UIText* text : mTexts) {
        text->Draw(renderer, mPos);
    }

    // --------------
    // TODO - PARTE 1-2
    // --------------

    // TODO 1.: Percorra a lista de botões (mButtons) e chame o método Draw de cada UIButton, passando o renderer
    for (UIButton* button : mButtons) {
        button->Draw(renderer, mPos);
    }

    // --------------
    // TODO - PARTE 1-3
    // --------------

    // TODO 1.: Percorra a lista de imagens (mImages) e chame o método Draw de cada UIImage, passando o renderer
    for (UIImage* image : mImages) {
        image->Draw(renderer, mPos);
    }

    DrawLifeBar(renderer);
    DrawManaBar(renderer);
}

void HUD::DrawLifeBar(class SDL_Renderer *renderer) {
    SDL_Rect HPBar;
    HPBar.x = static_cast<int>(mHPBar.x);
    HPBar.y = static_cast<int>(mHPBar.y);
    HPBar.w = static_cast<int>(mHPBar.w);
    HPBar.h = static_cast<int>(mHPBar.h);

    SDL_Rect DamageTakenBar;
    DamageTakenBar.x = static_cast<int>(mDamageTakenBar.x);
    DamageTakenBar.y = static_cast<int>(mDamageTakenBar.y);
    DamageTakenBar.w = static_cast<int>(mDamageTakenBar.w);
    DamageTakenBar.h = static_cast<int>(mDamageTakenBar.h);

    SDL_Rect HPRemainingBar;
    HPRemainingBar.x = static_cast<int>(mHPRemainingBar.x);
    HPRemainingBar.y = static_cast<int>(mHPRemainingBar.y);
    HPRemainingBar.w = static_cast<int>(mHPRemainingBar.w);
    HPRemainingBar.h = static_cast<int>(mHPRemainingBar.h);

    SDL_Rect HPGrowingBar;
    HPGrowingBar.x = static_cast<int>(mHPGrowingBar.x);
    HPGrowingBar.y = static_cast<int>(mHPGrowingBar.y);
    HPGrowingBar.w = static_cast<int>(mHPGrowingBar.w);
    HPGrowingBar.h = static_cast<int>(mHPGrowingBar.h);


    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 150);
    SDL_RenderFillRect(renderer, &HPBar);

    SDL_SetRenderDrawColor(renderer, 240, 234, 95, 255);
    SDL_RenderFillRect(renderer, &DamageTakenBar);

    SDL_SetRenderDrawColor(renderer, 242, 121, 123, 100);
    SDL_RenderFillRect(renderer, &HPRemainingBar);

    SDL_SetRenderDrawColor(renderer, 242, 90, 70, 255);
    SDL_RenderFillRect(renderer, &HPGrowingBar);
}

void HUD::DrawManaBar(struct SDL_Renderer *renderer) {
    SDL_Rect ManaBar;
    ManaBar.x = static_cast<int>(mManaBar.x);
    ManaBar.y = static_cast<int>(mManaBar.y);
    ManaBar.w = static_cast<int>(mManaBar.w);
    ManaBar.h = static_cast<int>(mManaBar.h);

    SDL_Rect ManaUsedBar;
    ManaUsedBar.x = static_cast<int>(mManaUsedBar.x);
    ManaUsedBar.y = static_cast<int>(mManaUsedBar.y);
    ManaUsedBar.w = static_cast<int>(mManaUsedBar.w);
    ManaUsedBar.h = static_cast<int>(mManaUsedBar.h);

    SDL_Rect ManaRemainingBar;
    ManaRemainingBar.x = static_cast<int>(mManaRemainingBar.x);
    ManaRemainingBar.y = static_cast<int>(mManaRemainingBar.y);
    ManaRemainingBar.w = static_cast<int>(mManaRemainingBar.w);
    ManaRemainingBar.h = static_cast<int>(mManaRemainingBar.h);


    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 150);
    SDL_RenderFillRect(renderer, &ManaBar);

    SDL_SetRenderDrawColor(renderer, 240, 234, 95, 255);
    SDL_RenderFillRect(renderer, &ManaUsedBar);

    SDL_SetRenderDrawColor(renderer, 65, 188, 217, 255);
    SDL_RenderFillRect(renderer, &ManaRemainingBar);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; i++) {
        SDL_RenderDrawLine(renderer,
                    mManaBar.x + i * mManaBar.w / 3,
                    mManaBar.y,
                    mManaBar.x + i * mManaBar.w / 3,
                    mManaBar.y + mManaBar.h);
    }
}

void HUD::ChangeResolution(float oldScale, float newScale) {
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

    mSpeedHPDecrease = mSpeedHPDecrease / oldScale * newScale;
    mSpeedHPIncrease = mSpeedHPIncrease / oldScale * newScale;

    mHPBar.x = mHPBar.x / oldScale * newScale;
    mHPBar.y = mHPBar.y / oldScale * newScale;
    mHPBar.w = mHPBar.w / oldScale * newScale;
    mHPBar.h = mHPBar.h / oldScale * newScale;

    mDamageTakenBar.x = mDamageTakenBar.x / oldScale * newScale;
    mDamageTakenBar.y = mDamageTakenBar.y / oldScale * newScale;
    mDamageTakenBar.w = mDamageTakenBar.w / oldScale * newScale;
    mDamageTakenBar.h = mDamageTakenBar.h / oldScale * newScale;

    mHPRemainingBar.x = mHPRemainingBar.x / oldScale * newScale;
    mHPRemainingBar.y = mHPRemainingBar.y / oldScale * newScale;
    mHPRemainingBar.w = mHPRemainingBar.w / oldScale * newScale;
    mHPRemainingBar.h = mHPRemainingBar.h / oldScale * newScale;

    mHPGrowingBar.x = mHPGrowingBar.x / oldScale * newScale;
    mHPGrowingBar.y = mHPGrowingBar.y / oldScale * newScale;
    mHPGrowingBar.w = mHPGrowingBar.w / oldScale * newScale;
    mHPGrowingBar.h = mHPGrowingBar.h / oldScale * newScale;

    mManaBar.x = mManaBar.x / oldScale * newScale;
    mManaBar.y = mManaBar.y / oldScale * newScale;
    mManaBar.w = mManaBar.w / oldScale * newScale;
    mManaBar.h = mManaBar.h / oldScale * newScale;

    mManaUsedBar.x = mManaUsedBar.x / oldScale * newScale;
    mManaUsedBar.y = mManaUsedBar.y / oldScale * newScale;
    mManaUsedBar.w = mManaUsedBar.w / oldScale * newScale;
    mManaUsedBar.h = mManaUsedBar.h / oldScale * newScale;

    mManaRemainingBar.x = mManaRemainingBar.x / oldScale * newScale;
    mManaRemainingBar.y = mManaRemainingBar.y / oldScale * newScale;
    mManaRemainingBar.w = mManaRemainingBar.w / oldScale * newScale;
    mManaRemainingBar.h = mManaRemainingBar.h / oldScale * newScale;
}
