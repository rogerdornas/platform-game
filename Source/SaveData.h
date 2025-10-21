//
// Created by roger on 28/09/2025.
//

#pragma once
#include "Game.h"

class SaveData
{
public:
    SaveData(class Game* game);

    void Save(const std::string& filename);
    bool Load(const std::string& filename);

    // injeta dados do save no jogo
    void ApplyToGame();
    void ApplyToPlayer();

    // captura dados atuais do jogo para o save
    void CaptureFromGame();

private:
    std::string GameSceneToString(Game::GameScene scene);
    Game::GameScene StringToGameScene(const std::string& str);

    class Game* mGame;
    Game::GameScene mGameScene;
    float mTotalPlayTime;

    Vector2 mLastCheckpointPosition;
    int mMoney;
    bool mCanDash;
    bool mCanFireBall;
    bool mCanWallSlide;
    int mMaxJumpsInAir;
    bool mCanHook;
    int mDeathCounter;

    bool mSwordRangeUpgrade;
    bool mSwordDamageUpgrade;
    bool mSwordSpeedUpgrade;
    bool mHealthPointsUpgrade;
    bool mHealCountUpgrade;
    bool mManaUpgrade;
    bool mFireballUpgrade;
};

