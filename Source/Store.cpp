//
// Created by roger on 18/06/2025.
//

#include "Store.h"
#include "Game.h"
#include "HUD.h"
#include "UIElements/UIText.h"

Store::Store(class Game *game, const std::string &fontName)
    :mGame(game)
    ,mFontName(fontName)
    ,mStoreMenu(nullptr)
    ,mStoreMessage(nullptr)
    ,mTextColor(Vector3(0.74f, 0.09f, 0.11f))
    ,mStoreOpened(false)
    ,mStoreMessageOpened(false)
    ,mSwordRangeUpgrade(false)
    ,mSwordRangeIncrease(1.2f)
    ,mSwordRangeUpgradeCost(50)
    ,mSwordDamageUpgrade(false)
    ,mSwordDamageIncrease(2.0f)
    ,mSwordDamageUpgradeCost(50)
    ,mSwordSpeedUpgrade(false)
    ,mSwordSpeedIncrease(2.0f)
    ,mSwordSpeedUpgradeCost(50)
    ,mHealthPointsUpgrade(false)
    ,mHealthPointsIncrease(30.0f)
    ,mHealthPointsUpgradeCost(150)
    ,mHealCountUpgrade(false)
    ,mHealCountUpgradeCost(250)
    ,mManaUpgrade(false)
    ,mManaIncrease(30.0f)
    ,mManaUpgradeCost(100)
    ,mFireballUpgrade(false)
    ,mFireballDamageIncrease(1.5f)
    ,mFireballSizeIncrease(1.5f)
    ,mFireballUpgradeCost(50)
{

}

Store::~Store() {

}

void Store::UpgradePlayerSwordRange() {
    mGame->GetPlayer()->SetSwordWidth(mGame->GetPlayer()->GetSword()->GetWidth() * mSwordRangeIncrease);
    mGame->GetPlayer()->GetSword()->SetWidth(mGame->GetPlayer()->GetSword()->GetWidth() * mSwordRangeIncrease);

    mGame->GetPlayer()->SetSwordHeight(mGame->GetPlayer()->GetSword()->GetHeight() * mSwordRangeIncrease);
    mGame->GetPlayer()->GetSword()->SetHeight(mGame->GetPlayer()->GetSword()->GetHeight() * mSwordRangeIncrease);

    mSwordRangeUpgrade = true;
}

void Store::UpgradePlayerSwordDamage() {
    mGame->GetPlayer()->SetSwordDamage(mGame->GetPlayer()->GetSword()->GetDamage() * mSwordDamageIncrease);
    mGame->GetPlayer()->GetSword()->SetDamage(mGame->GetPlayer()->GetSword()->GetDamage() * mSwordDamageIncrease);
    mSwordDamageUpgrade = true;
}

void Store::UpgradePlayerSwordSpeed() {
    mGame->GetPlayer()->AdjustSwordAttackSpeed(mSwordSpeedIncrease);
    mSwordSpeedUpgrade = true;
}

void Store::UpgradePlayerHealthPoints() {
    float hpPercentage = mGame->GetPlayer()->GetHealthPoints() / mGame->GetPlayer()->GetMaxHealthPoints();
    mGame->GetPlayer()->SetMaxHealthPoints(mGame->GetPlayer()->GetMaxHealthPoints() + mHealthPointsIncrease);
    mGame->GetPlayer()->SetHealthPoints(hpPercentage * mGame->GetPlayer()->GetMaxHealthPoints());
    // mGame->GetHUD()->IncreaseHPBar();
    mHealthPointsUpgrade = true;
}

void Store::UpgradePlayerHealCount() {
    mGame->GetPlayer()->IncreaseHealCount();
    mHealCountUpgrade = true;
}

void Store::UpgradePlayerMana() {
    mGame->GetPlayer()->SetMaxMana(mGame->GetPlayer()->GetMaxMana() + mManaIncrease);
    // mGame->GetHUD()->IncreaseManaBar();
    mManaUpgrade = true;
}

void Store::UpgradePlayerFireball() {
    mGame->GetPlayer()->SetFireballDamage(mGame->GetPlayer()->GetFireballDamage() * mFireballDamageIncrease);
    mGame->GetPlayer()->SetFireballWidth(mGame->GetPlayer()->GetFireballWidth() * mFireballSizeIncrease);
    mGame->GetPlayer()->SetFireballHeight(mGame->GetPlayer()->GetFireballHeight() * mFireballSizeIncrease);
    mFireballUpgrade = true;
}

void Store::OpenStore() {
    if (mStoreOpened) {
        return;
    }

    float virtualWidth = mGame->GetRenderer()->GetVirtualWidth();
    float virtualHeight = mGame->GetRenderer()->GetVirtualHeight();

    mStoreMenu = new UIScreen(mGame, mFontName);
    const Vector2 buttonSize = Vector2(virtualWidth * 0.35f, 75);
    mStoreMenu->SetSize(Vector2(virtualWidth / 2, 2 * virtualHeight / 3));
    mStoreMenu->SetPosition(Vector2(virtualWidth / 4, virtualHeight / 6));
    Vector2 buttonPos = Vector2(mStoreMenu->GetSize().x / 20, 0.0f);

    float costPosX = mStoreMenu->GetSize().x - 150;
    float moneyImgX = mStoreMenu->GetSize().x - 180;

    auto* background = mStoreMenu->AddImage("../Assets/Sprites/Menus/FundoPreto.png", mStoreMenu->GetSize() / 2, Vector2(virtualWidth, virtualHeight) * 1.5f);
    background->SetAlpha(0.5f);

    mStoreMenu->AddImage("../Assets/Sprites/Background/Store.png", mStoreMenu->GetSize() / 2, mStoreMenu->GetSize());

    UIText* text = mStoreMenu->AddText("LOJA", Vector2::Zero, Vector2::Zero, 40);
    text->SetPosition(Vector2(mStoreMenu->GetSize().x / 2, 40));

    int buttonPointSize = static_cast<int>(34);
    Vector2 textPos = Vector2(10, 0);

    if (mSwordRangeUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    std::string name = "AUMENTAR ALCANCE DA ESPADA";
    UIButton* button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mSwordRangeUpgrade && mGame->GetPlayer()->GetMoney() >= mSwordRangeUpgradeCost) {
            UpgradePlayerSwordRange();
            mGame->GetPlayer()->DecreaseMoney(mSwordRangeUpgradeCost);
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[0]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[0]->GetText()->SetText("AUMENTAR ALCANCE DA ESPADA");
            mStoreMenu->GetTexts()[1]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[1]->SetText(std::to_string(mSwordRangeUpgradeCost));
        }
    }, textPos, mTextColor);
    std::string cost = std::to_string(mSwordRangeUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mManaUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    name = "AUMENTAR MANA MÁXIMO";
    button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 2 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mManaUpgrade && mGame->GetPlayer()->GetMoney() >= mManaUpgradeCost) {
            UpgradePlayerMana();
            mGame->GetPlayer()->DecreaseMoney(mManaUpgradeCost);
            mGame->GetHUD()->IncreaseManaBar();
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[1]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[1]->GetText()->SetText("AUMENTAR MANA MÁXIMO");
            mStoreMenu->GetTexts()[2]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[2]->SetText(std::to_string(mManaUpgradeCost));
        }
    }, textPos, mTextColor);
    cost = std::to_string(mManaUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mHealthPointsUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    name = "AUMENTAR VIDA MÁXIMA";
    button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 3 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mHealthPointsUpgrade && mGame->GetPlayer()->GetMoney() >= mHealthPointsUpgradeCost) {
            UpgradePlayerHealthPoints();
            mGame->GetPlayer()->DecreaseMoney(mHealthPointsUpgradeCost);
            mGame->GetHUD()->IncreaseHPBar();
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[2]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[2]->GetText()->SetText("AUMENTAR VIDA MÁXIMA");
            mStoreMenu->GetTexts()[3]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[3]->SetText(std::to_string(mHealthPointsUpgradeCost));
        }
    }, textPos, mTextColor);
    cost = std::to_string(mHealthPointsUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mHealCountUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    name = "AUMENTAR NÚMERO DE CURAS";
    button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 4 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mHealCountUpgrade && mGame->GetPlayer()->GetMoney() >= mHealCountUpgradeCost) {
            UpgradePlayerHealCount();
            mGame->GetPlayer()->DecreaseMoney(mHealCountUpgradeCost);
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[3]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[3]->GetText()->SetText("AUMENTAR NÚMERO DE CURAS");
            mStoreMenu->GetTexts()[4]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[4]->SetText(std::to_string(mHealCountUpgradeCost));
        }
    }, textPos, mTextColor);
    cost = std::to_string(mHealCountUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mSwordSpeedUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    name = "AUMENTAR VELOCIDADE DA ESPADA";
    button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 5 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mSwordSpeedUpgrade && mGame->GetPlayer()->GetMoney() >= mSwordSpeedUpgradeCost) {
            UpgradePlayerSwordSpeed();
            mGame->GetPlayer()->DecreaseMoney(mSwordSpeedUpgradeCost);
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[4]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[4]->GetText()->SetText("AUMENTAR VELOCIDADE DA ESPADA");
            mStoreMenu->GetTexts()[5]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[5]->SetText(std::to_string(mSwordSpeedUpgradeCost));
        }
    }, textPos, mTextColor);
    cost = std::to_string(mSwordSpeedUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mSwordDamageUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    name = "AUMENTAR DANO DA ESPADA";
    button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 6 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
    [this]() {
        if (!mSwordDamageUpgrade && mGame->GetPlayer()->GetMoney() >= mSwordDamageUpgradeCost) {
            UpgradePlayerSwordDamage();
            mGame->GetPlayer()->DecreaseMoney(mSwordDamageUpgradeCost);
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[5]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[5]->GetText()->SetText("AUMENTAR DANO DA ESPADA");
            mStoreMenu->GetTexts()[6]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[6]->SetText(std::to_string(mSwordDamageUpgradeCost));
        }
    }, textPos, mTextColor);
    cost = std::to_string(mSwordDamageUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));

    if (mFireballUpgrade) {
        mTextColor = Color::Red;
    }
    else {
        mTextColor = Color::White;
    }
    if (mGame->GetPlayer()->GetCanFireBall()) {
        name = "MELHORAR BOLA DE FOGO";
        button = mStoreMenu->AddButton(name, buttonPos + Vector2(0, 7 * buttonSize.y), buttonSize, buttonPointSize, UIButton::TextPos::AlignLeft,
        [this]() {
            if (!mFireballUpgrade && mGame->GetPlayer()->GetMoney() >= mFireballUpgradeCost) {
                UpgradePlayerFireball();
                mGame->GetPlayer()->DecreaseMoney(mFireballUpgradeCost);
                mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
                mStoreMenu->GetButtons()[6]->GetText()->SetColor(Color::Red);
                mStoreMenu->GetButtons()[6]->GetText()->SetText("MELHORAR BOLA DE FOGO");
                mStoreMenu->GetTexts()[7]->SetColor(Color::Red);
                mStoreMenu->GetTexts()[7]->SetText(std::to_string(mFireballUpgradeCost));
            }
        }, textPos, mTextColor);
        cost = std::to_string(mFireballUpgradeCost);
        text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
        text->SetPosition(Vector2(costPosX + text->GetSize().x / 2, button->GetPosition().y + button->GetSize().y / 2));
        mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 2), Vector2(20, 35));
    }

    name = "FECHAR LOJA";
    mStoreMenu->AddButton(name, buttonPos + Vector2(0, mStoreMenu->GetSize().y - buttonSize.y * 1.2f), Vector2(virtualWidth * 0.45f, 75), buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        CloseStore();
    });

    if (mGame->GetIsPlayingOnKeyboard()) {
        text = mStoreMenu->AddText("PRESSIONE [ENTER] PARA COMPRAR", Vector2::Zero, Vector2::Zero, static_cast<int>(30));
    }
    else {
        text = mStoreMenu->AddText("PRESSIONE [A] PARA COMPRAR", Vector2::Zero, Vector2::Zero, static_cast<int>(30));
    }
    text->SetPosition(Vector2(mStoreMenu->GetSize().x / 2, 800));

    mStoreOpened = true;
    mGame->TogglePause();
}

void Store::CloseStore() {
    if (mStoreMenu != nullptr) {
        mStoreMenu->Close();
        mStoreOpened = false;
        mGame->TogglePause();
    }
}

void Store::LoadStoreMessage() {
    if (mStoreMessageOpened) {
        return;
    }

    if (mGame->GetGamePlayState() == Game::GamePlayState::Cutscene) {
        return;
    }

    float virtualWidth = mGame->GetRenderer()->GetVirtualWidth();
    float virtualHeight = mGame->GetRenderer()->GetVirtualHeight();

    mStoreMessage = new UIScreen(mGame, "../Assets/Fonts/K2D-Bold.ttf");
    mStoreMessage->SetSize(Vector2(virtualWidth / 3, 2 * virtualHeight / 6));
    mStoreMessage->SetPosition(Vector2(virtualWidth / 3, 7 * virtualHeight / 8));

    UIText* text;
    if (mGame->GetIsPlayingOnKeyboard()) {
        text = mStoreMessage->AddText("PRESSIONE [ESPAÇO] PARA ABRIR A LOJA", Vector2::Zero, Vector2::Zero, static_cast<int>(30));
    }
    else {
        text = mStoreMessage->AddText("PRESSIONE [Y] PARA ABRIR A LOJA", Vector2::Zero, Vector2::Zero, static_cast<int>(30));
    }
    text->SetPosition(Vector2(mStoreMessage->GetSize().x / 2, 35.0f));
    mStoreMessageOpened = true;
}

void Store::CloseStoreMessage() {
    if (mStoreMessage != nullptr) {
        mStoreMessage->Close();
    }
    mStoreMessageOpened = false;
}


void Store::ChangeResolution(float oldScale, float newScale) {
    mStoreMenu->ChangeResolution(oldScale, newScale);
}
