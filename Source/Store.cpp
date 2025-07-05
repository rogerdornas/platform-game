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
    ,mTextColor(Vector3(0.74, 0.09, 0.11))
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

void Store::OpenStore() {
    if (mStoreOpened) {
        return;
    }
    mStoreMenu = new UIScreen(mGame, mFontName);
    const Vector2 buttonSize = Vector2(mGame->GetLogicalWindowWidth() * 0.35, 75 * mGame->GetScale());
    mStoreMenu->SetSize(Vector2(mGame->GetLogicalWindowWidth() / 2, 2 * mGame->GetLogicalWindowHeight() / 3));
    mStoreMenu->SetPosition(Vector2(mGame->GetLogicalWindowWidth() / 4, mGame->GetLogicalWindowHeight() / 6));
    Vector2 buttonPos = Vector2(mStoreMenu->GetSize().x / 20, 0);

    float costPosX = mStoreMenu->GetSize().x - 150 * mGame->GetScale();
    float moneyImgX = mStoreMenu->GetSize().x - 180 * mGame->GetScale();

    mStoreMenu->AddImage("../Assets/Sprites/Background/Store.png", Vector2::Zero, mStoreMenu->GetSize());

    UIText* text = mStoreMenu->AddText("LOJA", Vector2::Zero, Vector2::Zero, 40 * mGame->GetScale());
    text->SetPosition(Vector2((mStoreMenu->GetSize().x - text->GetSize().x) / 2, 20 * mGame->GetScale()));

    int buttonPointSize = static_cast<int>(34 * mGame->GetScale());
    Vector2 textPos = Vector2(buttonSize.x / 20, buttonSize.y / 5);

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
            mGame->GetPlayer()->SetSwordWidth(mGame->GetPlayer()->GetSword()->GetWidth() * mSwordRangeIncrease);
            mGame->GetPlayer()->GetSword()->SetWidth(mGame->GetPlayer()->GetSword()->GetWidth() * mSwordRangeIncrease);

            mGame->GetPlayer()->SetSwordHeight(mGame->GetPlayer()->GetSword()->GetHeight() * mSwordRangeIncrease);
            mGame->GetPlayer()->GetSword()->SetHeight(mGame->GetPlayer()->GetSword()->GetHeight() * mSwordRangeIncrease);

            mGame->GetPlayer()->DecreaseMoney(mSwordRangeUpgradeCost);
            mSwordRangeUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[0]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[0]->GetText()->SetText("AUMENTAR ALCANCE DA ESPADA");
            mStoreMenu->GetTexts()[1]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[1]->SetText(std::to_string(mSwordRangeUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    std::string cost = std::to_string(mSwordRangeUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
            mGame->GetPlayer()->SetMaxMana(mGame->GetPlayer()->GetMaxMana() + mManaIncrease);
            mGame->GetHUD()->IncreaseManaBar();
            mGame->GetPlayer()->DecreaseMoney(mManaUpgradeCost);
            mManaUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[1]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[1]->GetText()->SetText("AUMENTAR MANA MÁXIMO");
            mStoreMenu->GetTexts()[2]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[2]->SetText(std::to_string(mManaUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    cost = std::to_string(mManaUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
            float hpPercentage = mGame->GetPlayer()->GetHealthPoints() / mGame->GetPlayer()->GetMaxHealthPoints();
            mGame->GetPlayer()->SetMaxHealthPoints(mGame->GetPlayer()->GetMaxHealthPoints() + mHealthPointsIncrease);
            mGame->GetPlayer()->SetHealthPoints(hpPercentage * mGame->GetPlayer()->GetMaxHealthPoints());
            mGame->GetHUD()->IncreaseHPBar();
            mGame->GetPlayer()->DecreaseMoney(mHealthPointsUpgradeCost);
            mHealthPointsUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[2]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[2]->GetText()->SetText("AUMENTAR VIDA MÁXIMA");
            mStoreMenu->GetTexts()[3]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[3]->SetText(std::to_string(mHealthPointsUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    cost = std::to_string(mHealthPointsUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
            mGame->GetPlayer()->IncreaseHealCount();
            mGame->GetPlayer()->DecreaseMoney(mHealCountUpgradeCost);
            mHealCountUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[3]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[3]->GetText()->SetText("AUMENTAR NÚMERO DE CURAS");
            mStoreMenu->GetTexts()[4]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[4]->SetText(std::to_string(mHealCountUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    cost = std::to_string(mHealCountUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
            mGame->GetPlayer()->AdjustSwordAttackSpeed(mSwordSpeedIncrease);
            mGame->GetPlayer()->DecreaseMoney(mSwordSpeedUpgradeCost);
            mSwordSpeedUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[4]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[4]->GetText()->SetText("AUMENTAR VELOCIDADE DA ESPADA");
            mStoreMenu->GetTexts()[5]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[5]->SetText(std::to_string(mSwordSpeedUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    cost = std::to_string(mSwordSpeedUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
            mGame->GetPlayer()->SetSwordDamage(mGame->GetPlayer()->GetSword()->GetDamage() * mSwordDamageIncrease);
            mGame->GetPlayer()->GetSword()->SetDamage(mGame->GetPlayer()->GetSword()->GetDamage() * mSwordDamageIncrease);
            mGame->GetPlayer()->DecreaseMoney(mSwordDamageUpgradeCost);
            mSwordDamageUpgrade = true;
            mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
            mStoreMenu->GetButtons()[5]->GetText()->SetColor(Color::Red);
            mStoreMenu->GetButtons()[5]->GetText()->SetText("AUMENTAR DANO DA ESPADA");
            mStoreMenu->GetTexts()[6]->SetColor(Color::Red);
            mStoreMenu->GetTexts()[6]->SetText(std::to_string(mSwordDamageUpgradeCost));
            // CloseStore();
            // OpenStore();
        }
    }, textPos, mTextColor);
    cost = std::to_string(mSwordDamageUpgradeCost);
    text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
    text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
    mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());

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
                mGame->GetPlayer()->SetFireballDamage(mGame->GetPlayer()->GetFireballDamage() * mFireballDamageIncrease);
                mGame->GetPlayer()->SetFireballWidth(mGame->GetPlayer()->GetFireballWidth() * mFireballSizeIncrease);
                mGame->GetPlayer()->SetFireballHeight(mGame->GetPlayer()->GetFireballHeight() * mFireballSizeIncrease);
                mGame->GetPlayer()->DecreaseMoney(mFireballUpgradeCost);
                mFireballUpgrade = true;
                mGame->GetAudio()->PlaySound("BuyItem/BuyItem.wav");
                mStoreMenu->GetButtons()[6]->GetText()->SetColor(Color::Red);
                mStoreMenu->GetButtons()[6]->GetText()->SetText("MELHORAR BOLA DE FOGO");
                mStoreMenu->GetTexts()[7]->SetColor(Color::Red);
                mStoreMenu->GetTexts()[7]->SetText(std::to_string(mFireballUpgradeCost));
                // CloseStore();
                // OpenStore();
            }
        }, textPos, mTextColor);
        cost = std::to_string(mFireballUpgradeCost);
        text = mStoreMenu->AddText(cost, Vector2::Zero, Vector2::Zero, buttonPointSize, mTextColor);
        text->SetPosition(Vector2(costPosX, button->GetPosition().y + button->GetSize().y / 5));
        mStoreMenu->AddImage("../Assets/Sprites/Money/CristalSmall.png", Vector2(moneyImgX, button->GetPosition().y + button->GetSize().y / 4), Vector2(20, 35) * mGame->GetScale());
    }

    name = "FECHAR LOJA";
    mStoreMenu->AddButton(name, buttonPos + Vector2(0, mStoreMenu->GetSize().y - buttonSize.y * 1.2f), Vector2(mGame->GetLogicalWindowWidth() * 0.45f, 75 * mGame->GetScale()), buttonPointSize, UIButton::TextPos::Center,
    [this]() {
        CloseStore();
    });

    if (mGame->GetIsPlayingOnKeyboard()) {
        text = mStoreMenu->AddText("PRESSIONE [ENTER] PARA COMPRAR", Vector2::Zero, Vector2::Zero, static_cast<int>(30 * mGame->GetScale()));
        text->SetPosition(Vector2(mGame->GetLogicalWindowWidth() / 12, 17 * mGame->GetLogicalWindowHeight() / 24));
    }
    else {
        text = mStoreMenu->AddText("PRESSIONE [A] PARA COMPRAR", Vector2::Zero, Vector2::Zero, static_cast<int>(30 * mGame->GetScale()));
        text->SetPosition(Vector2(mGame->GetLogicalWindowWidth() / 12, 17 * mGame->GetLogicalWindowHeight() / 24));
    }

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

    mStoreMessage = new UIScreen(mGame, "../Assets/Fonts/K2D-Bold.ttf");
    mStoreMessage->SetSize(Vector2(mGame->GetLogicalWindowWidth() / 3, 2 * mGame->GetLogicalWindowHeight() / 6));
    mStoreMessage->SetPosition(Vector2(mGame->GetLogicalWindowWidth() / 3, 7 * mGame->GetLogicalWindowHeight() / 8));

    if (mGame->GetIsPlayingOnKeyboard()) {
        mStoreMessage->AddText("PRESSIONE [ESPAÇO] PARA ABRIR A LOJA", Vector2::Zero, Vector2::Zero, static_cast<int>(30 * mGame->GetScale()));
    }
    else {
        mStoreMessage->AddText("PRESSIONE [Y] PARA ABRIR A LOJA", Vector2::Zero, Vector2::Zero, static_cast<int>(30 * mGame->GetScale()));
    }
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
