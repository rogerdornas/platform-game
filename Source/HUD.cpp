//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    ,mSpeedHPDecrease(200.0f * mGame->GetScale())
    ,mSpeedHPIncrease(200.0f * mGame->GetScale())
    ,mNumOfSubManaBars(mGame->GetPlayer()->GetMaxMana() / mGame->GetPlayer()->GetFireballManaCost())
    ,mWaitToDecreaseDuration(0.7f)
    ,mWaitToDecreaseTimer(0.0f)
    ,mBossWaitToDecreaseTimer(0.0f)
    ,mWaitToDecreaseManaDuration(0.7f)
    ,mWaitToDecreaseManaTimer(0.0f)
    ,mPlayerDie(false)
    ,mBossFight(false)
    ,mBoss(nullptr)
{
    float HPBarX = 50 * mGame->GetScale();
    float HPBarY = 50 * mGame->GetScale();
    float HPBarWidth = mGame->GetPlayer()->GetMaxHealthPoints() * 5 * mGame->GetScale();
    float HPBarHeight = 30 * mGame->GetScale();

    float ManaBarX = 50 * mGame->GetScale();
    float ManaBarY = 85 * mGame->GetScale();
    float ManaBarWidth = mGame->GetPlayer()->GetMaxMana() * 2.5 * mGame->GetScale();
    float ManaBarHeight = 30 * mGame->GetScale();

    float bossHPBarX = mGame->GetLogicalWindowWidth() * 0.15f;
    float bossHPBarY = mGame->GetLogicalWindowHeight() * 0.95f;
    float bossHPBarWidth = mGame->GetLogicalWindowWidth() * 0.7f;
    float bossHPBarHeight = 30 * mGame->GetScale();

    mHPBar = {HPBarX, HPBarY,HPBarWidth,HPBarHeight};
    mDamageTakenBar = mHPBar;
    mHPRemainingBar = mHPBar;
    mHPGrowingBar = mHPBar;

    mManaBar = {ManaBarX, ManaBarY, ManaBarWidth, ManaBarHeight};
    mManaUsedBar = mManaBar;
    mManaRemainingBar = mManaBar;

    mBossHPBar = {bossHPBarX, bossHPBarY,bossHPBarWidth,bossHPBarHeight};
    mBossDamageTakenBar = mBossHPBar;
    mBossHPRemainingBar = mBossHPBar;
    mBossHPGrowingBar = mBossHPBar;

    mPlayerHealCount = AddText(std::to_string(mGame->GetPlayer()->GetHealCount()),
                                Vector2(50, 120) * mGame->GetScale(),
                               Vector2(CHAR_WIDTH, WORD_HEIGHT) * mGame->GetScale(),
                                POINT_SIZE * mGame->GetScale());

    if (mGame->GetPlayer()->GetHealCount() == 0) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/empty.png", Vector2(80, 123) * mGame->GetScale(), Vector2(32, 32) * mGame->GetScale());
    }
    else if (mGame->GetPlayer()->GetHealCount() == 1) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/bemVazia.png", Vector2(80, 123) * mGame->GetScale(), Vector2(32, 32) * mGame->GetScale());
    }
    else if (mGame->GetPlayer()->GetHealCount() == 2) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/meioVazia.png", Vector2(80, 123) * mGame->GetScale(), Vector2(32, 32) * mGame->GetScale());
    }
    else if (mGame->GetPlayer()->GetHealCount() == 3 || mGame->GetPlayer()->GetHealCount() == 4) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/cheia.png", Vector2(80, 123) * mGame->GetScale(), Vector2(32, 32) * mGame->GetScale());
    }

    AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(1765, 52) * mGame->GetScale(), Vector2(20, 35) * mGame->GetScale());

    mPlayerMoney = AddText(std::to_string(mGame->GetPlayer()->GetMoney()),
                                Vector2(1800, 50) * mGame->GetScale(),
                               Vector2(CHAR_WIDTH, WORD_HEIGHT) * mGame->GetScale(),
                                POINT_SIZE * mGame->GetScale());
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

        if (mGame->GetPlayer()->GetHealCount() == 0) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/empty.png");
        }
        else if (mGame->GetPlayer()->GetHealCount() == 1) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/bemVazia.png");
        }
        else if (mGame->GetPlayer()->GetHealCount() == 2) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/meioVazia.png");
        }
        else if (mGame->GetPlayer()->GetHealCount() == 3 || mGame->GetPlayer()->GetHealCount() == 4) {
            mPotion->SetImage("../Assets/Sprites/Healingpotions/cheia.png");
        }

        std::string playerMoney = std::to_string(mGame->GetPlayer()->GetMoney());
        mPlayerMoney->SetText(playerMoney);
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

    // Boss HP bar
    if (mBossFight) {
        float bossHealthPoints = mBoss->GetHealthPoints() / mBoss->GetMaxHealthPoints();
        if (bossHealthPoints < 0) {
            bossHealthPoints = 0;
        }
        mBossHPRemainingBar.w = mBossHPBar.w * bossHealthPoints;
    }

    if (mBossHPGrowingBar.w < mBossHPRemainingBar.w) {
        mBossHPGrowingBar.w += mSpeedHPIncrease * deltaTime;
        if (mBossHPGrowingBar.w > mBossHPRemainingBar.w) {
            mBossHPGrowingBar.w = mBossHPRemainingBar.w;
        }
    }
    else {
        mBossHPGrowingBar.w = mBossHPRemainingBar.w;
    }

    if (mBossDamageTakenBar.w > mBossHPGrowingBar.w) {
        mBossWaitToDecreaseTimer += deltaTime;
        if (mBossWaitToDecreaseTimer >= mWaitToDecreaseDuration) {
            mBossDamageTakenBar.w -= mSpeedHPDecrease * deltaTime;
        }
    }
    else {
        mBossDamageTakenBar.w = mBossHPGrowingBar.w;
        mBossWaitToDecreaseTimer = 0;
    }

    if (mBoss && mBoss->GetState() == ActorState::Destroy) {
        EndBossFight();
    }
}

void HUD::StartBossFight(class Enemy *boss) {
    mBoss = boss;
    mBossFight = true;
}

void HUD::EndBossFight() {
    mBossFight = false;
    mBoss = nullptr;
    mBossDamageTakenBar = mBossHPBar;
    mBossHPRemainingBar = mBossHPBar;
    mBossHPGrowingBar = mBossHPBar;
}


void HUD::IncreaseHPBar() {
    mHPBar.w = mGame->GetPlayer()->GetMaxHealthPoints() * 5 * mGame->GetScale();
}

void HUD::IncreaseManaBar() {
    mManaBar.w = mGame->GetPlayer()->GetMaxMana() * 2.5 * mGame->GetScale();
    mNumOfSubManaBars = mGame->GetPlayer()->GetMaxMana() / mGame->GetPlayer()->GetFireballManaCost();
}

void HUD::Draw(class SDL_Renderer *renderer) {
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

    DrawLifeBar(renderer);
    DrawManaBar(renderer);
    if (mBossFight) {
        DrawBossLifeBar(renderer);
    }
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
    for (int i = 1; i < mNumOfSubManaBars; i++) {
        SDL_RenderDrawLine(renderer,
                    mManaBar.x + i * mManaBar.w / mNumOfSubManaBars,
                    mManaBar.y,
                    mManaBar.x + i * mManaBar.w / mNumOfSubManaBars,
                    mManaBar.y + mManaBar.h);
    }
}

void HUD::DrawBossLifeBar(struct SDL_Renderer *renderer) {
    SDL_Rect HPBar;
    HPBar.x = static_cast<int>(mBossHPBar.x);
    HPBar.y = static_cast<int>(mBossHPBar.y);
    HPBar.w = static_cast<int>(mBossHPBar.w);
    HPBar.h = static_cast<int>(mBossHPBar.h);

    SDL_Rect DamageTakenBar;
    DamageTakenBar.x = static_cast<int>(mBossDamageTakenBar.x);
    DamageTakenBar.y = static_cast<int>(mBossDamageTakenBar.y);
    DamageTakenBar.w = static_cast<int>(mBossDamageTakenBar.w);
    DamageTakenBar.h = static_cast<int>(mBossDamageTakenBar.h);

    SDL_Rect HPRemainingBar;
    HPRemainingBar.x = static_cast<int>(mBossHPRemainingBar.x);
    HPRemainingBar.y = static_cast<int>(mBossHPRemainingBar.y);
    HPRemainingBar.w = static_cast<int>(mBossHPRemainingBar.w);
    HPRemainingBar.h = static_cast<int>(mBossHPRemainingBar.h);

    SDL_Rect HPGrowingBar;
    HPGrowingBar.x = static_cast<int>(mBossHPGrowingBar.x);
    HPGrowingBar.y = static_cast<int>(mBossHPGrowingBar.y);
    HPGrowingBar.w = static_cast<int>(mBossHPGrowingBar.w);
    HPGrowingBar.h = static_cast<int>(mBossHPGrowingBar.h);

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 150);
    SDL_RenderFillRect(renderer, &HPBar);

    SDL_SetRenderDrawColor(renderer, 240, 234, 95, 255);
    SDL_RenderFillRect(renderer, &DamageTakenBar);

    SDL_SetRenderDrawColor(renderer, 242, 121, 123, 100);
    SDL_RenderFillRect(renderer, &HPRemainingBar);

    SDL_SetRenderDrawColor(renderer, 242, 90, 70, 255);
    SDL_RenderFillRect(renderer, &HPGrowingBar);
}


void HUD::ChangeResolution(float oldScale, float newScale) {
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

    mBossHPBar.x = mBossHPBar.x / oldScale * newScale;
    mBossHPBar.y = mBossHPBar.y / oldScale * newScale;
    mBossHPBar.w = mBossHPBar.w / oldScale * newScale;
    mBossHPBar.h = mBossHPBar.h / oldScale * newScale;

    mBossDamageTakenBar.x = mBossDamageTakenBar.x / oldScale * newScale;
    mBossDamageTakenBar.y = mBossDamageTakenBar.y / oldScale * newScale;
    mBossDamageTakenBar.w = mBossDamageTakenBar.w / oldScale * newScale;
    mBossDamageTakenBar.h = mBossDamageTakenBar.h / oldScale * newScale;

    mBossHPRemainingBar.x = mBossHPRemainingBar.x / oldScale * newScale;
    mBossHPRemainingBar.y = mBossHPRemainingBar.y / oldScale * newScale;
    mBossHPRemainingBar.w = mBossHPRemainingBar.w / oldScale * newScale;
    mBossHPRemainingBar.h = mBossHPRemainingBar.h / oldScale * newScale;

    mBossHPGrowingBar.x = mBossHPGrowingBar.x / oldScale * newScale;
    mBossHPGrowingBar.y = mBossHPGrowingBar.y / oldScale * newScale;
    mBossHPGrowingBar.w = mBossHPGrowingBar.w / oldScale * newScale;
    mBossHPGrowingBar.h = mBossHPGrowingBar.h / oldScale * newScale;
}
