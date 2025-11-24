//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "Game.h"
#include "UIElements/UIText.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    ,mSpeedHPDecrease(200.0f)
    ,mSpeedHPIncrease(200.0f)
    ,mNumOfSubManaBars(mGame->GetPlayer()->GetMaxMana() / mGame->GetPlayer()->GetFireballManaCost())
    ,mWaitToDecreaseDuration(0.7f)
    ,mWaitToDecreaseTimer(0.0f)
    ,mWaitToDecreaseManaDuration(0.7f)
    ,mWaitToDecreaseManaTimer(0.0f)
    ,mPlayerDie(false)
{
    float HPBarX = 50;
    float HPBarY = 50;
    float HPBarWidth = mGame->GetPlayer()->GetMaxHealthPoints() * 5;
    float HPBarHeight = 30;

    float ManaBarX = 50;
    float ManaBarY = 85;
    float ManaBarWidth = mGame->GetPlayer()->GetMaxMana() * 2.5;
    float ManaBarHeight = 30;

    float bossHPBarX = mGame->GetRenderer()->GetVirtualWidth() * 0.15f;
    float bossHPBarY = mGame->GetRenderer()->GetVirtualHeight() * 0.95f;
    float bossHPBarWidth = mGame->GetRenderer()->GetVirtualWidth() * 0.7f;
    float bossHPBarHeight = 30;

    mHPBar = {HPBarX, HPBarY,HPBarWidth,HPBarHeight};
    mDamageTakenBar = mHPBar;
    mDamageTakenBar.w = 0;
    mHPRemainingBar = mHPBar;
    mHPGrowingBar = mHPBar;

    mManaBar = {ManaBarX, ManaBarY, ManaBarWidth, ManaBarHeight};
    mManaUsedBar = mManaBar;
    mManaUsedBar.w = 0;
    mManaRemainingBar = mManaBar;

    mBossHPBar = {bossHPBarX, bossHPBarY,bossHPBarWidth,bossHPBarHeight};
    mBossDamageTakenBar = mBossHPBar;
    mBossHPRemainingBar = mBossHPBar;
    mBossHPGrowingBar = mBossHPBar;

    mPlayerHealCount = AddText(std::to_string(mGame->GetPlayer()->GetHealCount()),
                                Vector2(65, 138),
                               Vector2(CHAR_WIDTH, WORD_HEIGHT),
                                POINT_SIZE);

    if (mGame->GetPlayer()->GetHealCount() == 0) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/empty.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (mGame->GetPlayer()->GetHealCount() == 1) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/bemVazia.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (mGame->GetPlayer()->GetHealCount() == 2) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/meioVazia.png", Vector2(96, 139), Vector2(32, 32));
    }
    else if (mGame->GetPlayer()->GetHealCount() == 3 || mGame->GetPlayer()->GetHealCount() == 4) {
        mPotion = AddImage("../Assets/Sprites/Healingpotions/cheia.png", Vector2(96, 139), Vector2(32, 32));
    }

    AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(1770, 68), Vector2(18.0f, 31.5f));

    mPlayerMoney = AddText(std::to_string(mGame->GetPlayer()->GetMoney()),
                                Vector2::Zero,
                               Vector2(CHAR_WIDTH, WORD_HEIGHT),
                                POINT_SIZE);
    mPlayerMoney->SetPosition(Vector2(1790 + mPlayerMoney->GetSize().x / 2, 65));
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
        mPlayerMoney->SetPosition(Vector2(1790 + mPlayerMoney->GetSize().x / 2, 65));
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

    // Boss HP bars
    for (int i = 0; i < mBossLifeBars.size(); i++) {
        // deslocamento das barras de boss
        mBossLifeBars[i].bossHPBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossDamageTakenBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossHPRemainingBar.y = mBossHPBar.y - 50 * i;
        mBossLifeBars[i].bossHPGrowingBar.y = mBossHPBar.y - 50 * i;

        float bossHealthPoints = mBossLifeBars[i].boss->GetHealthPoints() / mBossLifeBars[i].boss->GetMaxHealthPoints();
        if (bossHealthPoints < 0) {
            bossHealthPoints = 0;
        }
        mBossLifeBars[i].bossHPRemainingBar.w = mBossLifeBars[i].bossHPBar.w * bossHealthPoints;

        if (mBossLifeBars[i].bossHPGrowingBar.w < mBossLifeBars[i].bossHPRemainingBar.w) {
            mBossLifeBars[i].bossHPGrowingBar.w += mSpeedHPIncrease * deltaTime;
            if (mBossLifeBars[i].bossHPGrowingBar.w > mBossLifeBars[i].bossHPRemainingBar.w) {
                mBossLifeBars[i].bossHPGrowingBar.w = mBossLifeBars[i].bossHPRemainingBar.w;
            }
        }
        else {
            mBossLifeBars[i].bossHPGrowingBar.w = mBossLifeBars[i].bossHPRemainingBar.w;
        }

        if (mBossLifeBars[i].bossDamageTakenBar.w > mBossLifeBars[i].bossHPGrowingBar.w) {
            mBossLifeBars[i].waitToDecreaseTimer += deltaTime;
            if (mBossLifeBars[i].waitToDecreaseTimer >= mWaitToDecreaseDuration) {
                mBossLifeBars[i].bossDamageTakenBar.w -= mSpeedHPDecrease * deltaTime;
            }
        }
        else {
            mBossLifeBars[i].bossDamageTakenBar.w = mBossLifeBars[i].bossHPGrowingBar.w;
            mBossLifeBars[i].waitToDecreaseTimer = 0;
        }

        if (mBossLifeBars[i].boss && mBossLifeBars[i].boss->GetState() == ActorState::Destroy) {
            EndBossFight(mBossLifeBars[i].boss);
            i--;
        }
    }
}

void HUD::StartBossFight(class Enemy *boss) {
    for (auto it = mBossLifeBars.begin(); it != mBossLifeBars.end(); ) {
        if (it->boss == boss) {
            return;
        }
        else {
            ++it;
        }
    }

    BossLifeBar bossLifeBar;
    bossLifeBar.bossHPBar = mBossHPBar;
    bossLifeBar.bossDamageTakenBar = mBossDamageTakenBar;
    bossLifeBar.bossHPRemainingBar = mBossHPRemainingBar;
    bossLifeBar.bossHPGrowingBar = mBossHPGrowingBar;
    bossLifeBar.boss = boss;
    bossLifeBar.waitToDecreaseTimer = 0.0f;

    mBossLifeBars.emplace_back(bossLifeBar);
}

void HUD::EndBossFight(class Enemy* boss) {
    for (auto it = mBossLifeBars.begin(); it != mBossLifeBars.end(); ) {
        if (it->boss == boss) {
            it = mBossLifeBars.erase(it);
            break;
        } else {
            // Só avança se não tiver apagado nada
            ++it;
        }
    }
}


void HUD::IncreaseHPBar() {
    mHPBar.w = mGame->GetPlayer()->GetMaxHealthPoints() * 5;
}

void HUD::IncreaseManaBar() {
    mManaBar.w = mGame->GetPlayer()->GetMaxMana() * 2.5;
    mNumOfSubManaBars = mGame->GetPlayer()->GetMaxMana() / mGame->GetPlayer()->GetFireballManaCost();
}

void HUD::Draw(Renderer *renderer) {
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
    if (!mBossLifeBars.empty()) {
        DrawBossLifeBar(renderer);
    }
}

void HUD::DrawLifeBar(Renderer *renderer) {
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

    renderer->DrawRect(Vector2(HPBar.x, HPBar.y) + Vector2(HPBar.w, HPBar.h) / 2, Vector2(HPBar.w, HPBar.h), 0.0f,
                         Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

    renderer->DrawRect(Vector2(DamageTakenBar.x, DamageTakenBar.y) + Vector2(DamageTakenBar.w, DamageTakenBar.h) / 2, Vector2(DamageTakenBar.w, DamageTakenBar.h), 0.0f,
                     Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    renderer->DrawRect(Vector2(HPRemainingBar.x, HPRemainingBar.y) + Vector2(HPRemainingBar.w, HPRemainingBar.h) / 2, Vector2(HPRemainingBar.w, HPRemainingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 121 / 255.0f, 123 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 100 / 255.0f);

    renderer->DrawRect(Vector2(HPGrowingBar.x, HPGrowingBar.y) + Vector2(HPGrowingBar.w, HPGrowingBar.h) / 2, Vector2(HPGrowingBar.w, HPGrowingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 90 / 255.0f, 70 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

}

void HUD::DrawManaBar(Renderer *renderer) {
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

    renderer->DrawRect(Vector2(ManaBar.x, ManaBar.y) + Vector2(ManaBar.w, ManaBar.h) / 2, Vector2(ManaBar.w, ManaBar.h), 0.0f,
                 Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

    renderer->DrawRect(Vector2(ManaUsedBar.x, ManaUsedBar.y) + Vector2(ManaUsedBar.w, ManaUsedBar.h) / 2, Vector2(ManaUsedBar.w, ManaUsedBar.h), 0.0f,
             Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    renderer->DrawRect(Vector2(ManaRemainingBar.x, ManaRemainingBar.y) + Vector2(ManaRemainingBar.w, ManaRemainingBar.h) / 2, Vector2(ManaRemainingBar.w, ManaRemainingBar.h), 0.0f,
             Vector3(65 / 255.0f, 188 / 255.0f, 217 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

    for (int i = 1; i < mNumOfSubManaBars; i++) {
        renderer->DrawLine(Vector2(mManaBar.x + i * mManaBar.w / mNumOfSubManaBars, mManaBar.y),
                           Vector2(mManaBar.x + i * mManaBar.w / mNumOfSubManaBars, mManaBar.y + mManaBar.h),
                           Vector3(0, 0, 0), 2.0f, Vector2::Zero, 1.0f);
    }
}

void HUD::DrawBossLifeBar(Renderer *renderer) {
    for (int i = 0; i < mBossLifeBars.size(); i++) {
        SDL_Rect HPBar;
        HPBar.x = static_cast<int>(mBossLifeBars[i].bossHPBar.x);
        HPBar.y = static_cast<int>(mBossLifeBars[i].bossHPBar.y);
        HPBar.w = static_cast<int>(mBossLifeBars[i].bossHPBar.w);
        HPBar.h = static_cast<int>(mBossLifeBars[i].bossHPBar.h);

        SDL_Rect DamageTakenBar;
        DamageTakenBar.x = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.x);
        DamageTakenBar.y = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.y);
        DamageTakenBar.w = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.w);
        DamageTakenBar.h = static_cast<int>(mBossLifeBars[i].bossDamageTakenBar.h);

        SDL_Rect HPRemainingBar;
        HPRemainingBar.x = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.x);
        HPRemainingBar.y = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.y);
        HPRemainingBar.w = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.w);
        HPRemainingBar.h = static_cast<int>(mBossLifeBars[i].bossHPRemainingBar.h);

        SDL_Rect HPGrowingBar;
        HPGrowingBar.x = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.x);
        HPGrowingBar.y = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.y);
        HPGrowingBar.w = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.w);
        HPGrowingBar.h = static_cast<int>(mBossLifeBars[i].bossHPGrowingBar.h);

        renderer->DrawRect(Vector2(HPBar.x, HPBar.y) + Vector2(HPBar.w, HPBar.h) / 2, Vector2(HPBar.w, HPBar.h), 0.0f,
                         Vector3(40 / 255.0f, 40 / 255.0f, 40 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 150 / 255.0f);

        renderer->DrawRect(Vector2(DamageTakenBar.x, DamageTakenBar.y) + Vector2(DamageTakenBar.w, DamageTakenBar.h) / 2, Vector2(DamageTakenBar.w, DamageTakenBar.h), 0.0f,
                     Vector3(240 / 255.0f, 234 / 255.0f, 95 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);

        renderer->DrawRect(Vector2(HPRemainingBar.x, HPRemainingBar.y) + Vector2(HPRemainingBar.w, HPRemainingBar.h) / 2, Vector2(HPRemainingBar.w, HPRemainingBar.h), 0.0f,
                     Vector3(242 / 255.0f, 121 / 255.0f, 123 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 100 / 255.0f);

        renderer->DrawRect(Vector2(HPGrowingBar.x, HPGrowingBar.y) + Vector2(HPGrowingBar.w, HPGrowingBar.h) / 2, Vector2(HPGrowingBar.w, HPGrowingBar.h), 0.0f,
                 Vector3(242 / 255.0f, 90 / 255.0f, 70 / 255.0f), Vector2::Zero, RendererMode::TRIANGLES, 255 / 255.0f);
    }
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
