//
// Created by roger on 18/06/2025.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"

class Store
{
public:
    Store(class Game* game, const std::string& fontName);
    ~Store();

    void OpenStore();
    void CloseStore();
    void LoadStoreMessage();
    void CloseStoreMessage();

    bool StoreOpened() const { return mStoreOpened; }
    bool StoreMessageOpened() const { return mStoreMessageOpened; }

    void UpgradePlayerSwordRange();
    void UpgradePlayerSwordDamage();
    void UpgradePlayerSwordSpeed();
    void UpgradePlayerHealthPoints();
    void UpgradePlayerHealCount();
    void UpgradePlayerMana();
    void UpgradePlayerFireball();

    bool GetSwordRangeUpgrade() const { return mSwordRangeUpgrade; }
    bool GetSwordDamageUpgrade() const { return mSwordDamageUpgrade; }
    bool GetSwordSpeedUpgrade() const { return mSwordSpeedUpgrade; }
    bool GetHealthPointsUpgrade() const { return mHealthPointsUpgrade; }
    bool GetHealCountUpgrade() const { return mHealCountUpgrade; }
    bool GetManaUpgrade() const { return mManaUpgrade; }
    bool GetFireballUpgrade() const { return mFireballUpgrade; }

    void ChangeResolution(float oldScale, float newScale);

private:
    class Game* mGame;
    std::string mFontName;
    UIScreen* mStoreMenu;
    UIScreen* mStoreMessage;
    Vector3 mTextColor;

    bool mStoreOpened;
    bool mStoreMessageOpened;

    bool mSwordRangeUpgrade;
    float mSwordRangeIncrease;
    int mSwordRangeUpgradeCost;

    bool mSwordDamageUpgrade;
    float mSwordDamageIncrease;
    int mSwordDamageUpgradeCost;

    bool mSwordSpeedUpgrade;
    float mSwordSpeedIncrease;
    int mSwordSpeedUpgradeCost;

    bool mHealthPointsUpgrade;
    float mHealthPointsIncrease;
    int mHealthPointsUpgradeCost;

    bool mHealCountUpgrade;
    int mHealCountUpgradeCost;

    bool mManaUpgrade;
    float mManaIncrease;
    int mManaUpgradeCost;

    bool mFireballUpgrade;
    float mFireballDamageIncrease;
    float mFireballSizeIncrease;
    int mFireballUpgradeCost;
};
