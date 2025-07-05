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

    void IncreaseHPBar();
    void IncreaseManaBar();

    void Draw(class SDL_Renderer *renderer) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
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

    // SDL_Rect mHPBar;
    // SDL_Rect mDamageTakenBar;
    // SDL_Rect mHPRemainingBar;
    // SDL_Rect mHPGrowingBar;
    //
    // SDL_Rect mManaBar;
    // SDL_Rect mManaUsedBar;
    // SDL_Rect mManaRemainingBar;

    float mWaitToDecreaseDuration;
    float mWaitToDecreaseTimer;
    float mWaitToDecreaseManaDuration;
    float mWaitToDecreaseManaTimer;
    bool mPlayerDie;

    void DrawLifeBar(class SDL_Renderer *renderer);
    void DrawManaBar(class SDL_Renderer * renderer);
};
