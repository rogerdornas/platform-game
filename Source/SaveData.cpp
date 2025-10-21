//
// Created by roger on 28/09/2025.
//

#include "SaveData.h"
#include "Json.h"
#include <fstream>

SaveData::SaveData(class Game *game)
    :mGame(game)
    ,mGameScene(Game::GameScene::Prologue)
    ,mLastCheckpointPosition(Vector2(1952, 4352))
    ,mMoney(0)
    ,mCanDash(false)
    ,mCanFireBall(false)
    ,mCanWallSlide(false)
    ,mMaxJumpsInAir(0)
    ,mCanHook(false)
    ,mDeathCounter(0)
    ,mSwordRangeUpgrade(false)
    ,mSwordDamageUpgrade(false)
    ,mSwordSpeedUpgrade(false)
    ,mHealthPointsUpgrade(false)
    ,mHealCountUpgrade(false)
    ,mManaUpgrade(false)
    ,mFireballUpgrade(false)
{
}

void SaveData::Save(const std::string &filename) {
    nlohmann::json j;

    j["game"]["scene"] = GameSceneToString(mGameScene);
    j["game"]["last_checkpoint"] = { {"x", mLastCheckpointPosition.x}, {"y", mLastCheckpointPosition.y} };

    j["player"] = {
        {"can_dash", mCanDash},
        {"money", mMoney},
        {"max_jumps_in_air", mMaxJumpsInAir},
        {"can_fireball", mCanFireBall},
        {"can_wallslide", mCanWallSlide},
        {"can_hook", mCanHook},
        {"death_counter", mDeathCounter}
    };

    j["upgrades"] = {
        {"sword_range", mSwordRangeUpgrade},
        {"sword_damage", mSwordDamageUpgrade},
        {"sword_speed", mSwordSpeedUpgrade},
        {"health_points", mHealthPointsUpgrade},
        {"heal_count", mHealCountUpgrade},
        {"mana", mManaUpgrade},
        {"fireball", mFireballUpgrade}
    };

    std::ofstream file(filename);
    file << j.dump(4); // 4 = identação
}

bool SaveData::Load(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json j;
    file >> j;

    mGameScene = StringToGameScene(j["game"]["scene"]);
    mLastCheckpointPosition.x = j["game"]["last_checkpoint"]["x"];
    mLastCheckpointPosition.y = j["game"]["last_checkpoint"]["y"];

    mMoney = j["player"]["money"];
    mCanDash = j["player"]["can_dash"];
    mMaxJumpsInAir = j["player"]["max_jumps_in_air"];
    mCanFireBall = j["player"]["can_fireball"];
    mCanWallSlide = j["player"]["can_wallslide"];
    mCanHook = j["player"]["can_hook"];
    mDeathCounter = j["player"]["death_counter"];

    mSwordRangeUpgrade = j["upgrades"]["sword_range"];
    mSwordDamageUpgrade = j["upgrades"]["sword_damage"];
    mSwordSpeedUpgrade = j["upgrades"]["sword_speed"];
    mHealthPointsUpgrade = j["upgrades"]["health_points"];
    mHealCountUpgrade = j["upgrades"]["heal_count"];
    mManaUpgrade = j["upgrades"]["mana"];
    mFireballUpgrade = j["upgrades"]["fireball"];

    return true;
}

std::string SaveData::GameSceneToString(Game::GameScene scene) {
    switch (scene) {
        case Game::GameScene::LevelTeste: return "LevelTeste";
        case Game::GameScene::Coliseu: return "Coliseu";
        case Game::GameScene::Prologue: return "Prologue";
        case Game::GameScene::Level1: return "Level1";
        case Game::GameScene::Level2: return "Level2";
        case Game::GameScene::Level3: return "Level3";
        case Game::GameScene::Level4: return "Level4";
        case Game::GameScene::Level5: return "Level5";
        case Game::GameScene::Room0: return "Room0";
        case Game::GameScene::MirrorBoss: return "MirrorBoss";
        default: return "Unknown";
    }
}

Game::GameScene SaveData::StringToGameScene(const std::string &str) {
    if (str == "LevelTeste") return Game::GameScene::LevelTeste;
    if (str == "Coliseu") return Game::GameScene::Coliseu;
    if (str == "Prologue") return Game::GameScene::Prologue;
    if (str == "Level1") return Game::GameScene::Level1;
    if (str == "Level2") return Game::GameScene::Level2;
    if (str == "Level3") return Game::GameScene::Level3;
    if (str == "Level4") return Game::GameScene::Level4;
    if (str == "Level5") return Game::GameScene::Level5;
    if (str == "Room0") return Game::GameScene::Room0;
    if (str == "MirrorBoss") return Game::GameScene::MirrorBoss;
    return Game::GameScene::Level1; // fallback
}

void SaveData::ApplyToGame() {
    mGame->SetGameScene(mGameScene, 0.5f);
}

void SaveData::ApplyToPlayer() {
    Player* player = mGame->GetPlayer();
    player->SetPosition(mLastCheckpointPosition * mGame->GetScale());
    player->SetStartingPosition(mLastCheckpointPosition * mGame->GetScale());
    player->SetMoney(mMoney);
    player->SetCanDash(mCanDash);
    player->SetMaxJumpsInAir(mMaxJumpsInAir);
    player->SetCanFireBall(mCanFireBall);
    player->SetCanWallSlide(mCanWallSlide);
    player->SetCanHook(mCanHook);
    player->SetDeathCounter(mDeathCounter);

    Store* store = mGame->GetStore();
    if (mSwordRangeUpgrade) {
        store->UpgradePlayerSwordRange();
    }
    if (mSwordDamageUpgrade) {
        store->UpgradePlayerSwordDamage();
    }
    if (mSwordSpeedUpgrade) {
        store->UpgradePlayerSwordSpeed();
    }
    if (mHealthPointsUpgrade) {
        store->UpgradePlayerHealthPoints();
    }
    if (mHealCountUpgrade) {
        store->UpgradePlayerHealCount();
    }
    if (mManaUpgrade) {
        store->UpgradePlayerMana();
    }
    if (mFireballUpgrade) {
        store->UpgradePlayerFireball();
    }
}

void SaveData::CaptureFromGame() {
    Player* player = mGame->GetPlayer();
    mGameScene = mGame->GetCheckpointGameScene();
    mLastCheckpointPosition.x = mGame->GetCheckPointPosition().x / mGame->GetScale();
    mLastCheckpointPosition.y = mGame->GetCheckPointPosition().y / mGame->GetScale();
    mMoney = player->GetMoney();
    mCanDash = player->GetCanDash();
    mMaxJumpsInAir = player->GetMaxJumpsInAir();
    mCanFireBall = player->GetCanFireBall();
    mCanWallSlide = player->GetCanWallSlide();
    mCanHook = player->GetCanHook();
    mDeathCounter = player->GetDeathCounter();

    Store* store = mGame->GetStore();
    mSwordRangeUpgrade = store->GetSwordRangeUpgrade();
    mSwordDamageUpgrade = store->GetSwordDamageUpgrade();
    mSwordSpeedUpgrade = store->GetSwordSpeedUpgrade();
    mHealthPointsUpgrade = store->GetHealthPointsUpgrade();
    mHealCountUpgrade = store->GetHealCountUpgrade();
    mManaUpgrade = store->GetManaUpgrade();
    mFireballUpgrade = store->GetFireballUpgrade();
}
