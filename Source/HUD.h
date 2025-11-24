//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>
#include "UIElements/UIScreen.h"

class HUD : public UIScreen
{
public:
    struct RectF {
        float x, y, w, h;
    };

    const int POINT_SIZE = 30;
    const int WORD_HEIGHT = 30.0f;
    const int WORD_OFFSET = 25.0f;
    const int CHAR_WIDTH = 30.0f;
    const int HUD_POS_Y = 10.0f;

    HUD(class Game* game, const std::string& fontName);
    ~HUD();

    void Update(float deltaTime) override;

    void StartBossFight(class Enemy* boss);
    void EndBossFight(class Enemy* boss);

    void IncreaseHPBar();
    void IncreaseManaBar();

    void Draw(class Renderer *renderer) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    struct BossLifeBar {
        RectF bossHPBar;
        RectF bossDamageTakenBar;
        RectF bossHPRemainingBar;
        RectF bossHPGrowingBar;
        Enemy* boss;
        float waitToDecreaseTimer;
    };

    // HUD elements
    UIText* mPlayerHealCount;
    UIText* mPlayerMoney;
    UIImage* mPotion;

    float mSpeedHPDecrease;
    float mSpeedHPIncrease;

    RectF mHPBar;
    RectF mDamageTakenBar;
    RectF mHPRemainingBar;
    RectF mHPGrowingBar;

    RectF mManaBar;
    RectF mManaUsedBar;
    RectF mManaRemainingBar;
    int mNumOfSubManaBars;

    RectF mBossHPBar;
    RectF mBossDamageTakenBar;
    RectF mBossHPRemainingBar;
    RectF mBossHPGrowingBar;

    std::vector<BossLifeBar> mBossLifeBars;

    float mWaitToDecreaseDuration;
    float mWaitToDecreaseTimer;
    float mWaitToDecreaseManaDuration;
    float mWaitToDecreaseManaTimer;
    bool mPlayerDie;

    void DrawLifeBar(class Renderer *renderer);
    void DrawManaBar(class Renderer * renderer);
    void DrawBossLifeBar(class Renderer * renderer);
};
