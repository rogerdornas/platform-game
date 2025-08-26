//
// Created by roger on 22/08/2025.
//

#include "WaveManager.h"
#include "Game.h"
#include <fstream>
#include "Json.h"
#include "Actors/DragonFly.h"
#include "Actors/DynamicGround.h"
#include "Actors/EnemySimple.h"
#include "Actors/FlyingEnemySimple.h"
#include "Actors/FlyingGolem.h"
#include "Actors/FlyingShooterEnemy.h"
#include "Actors/Fox.h"
#include "Actors/Mantis.h"

WaveManager::WaveManager(Game *game)
    :mGame(game)
    ,mCurrentWave(-1)
    ,mWaveTimer(0.0f)
    ,mStarted(false)
{
}

bool WaveManager::LoadFromJson(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json j;
    file >> j;

    for (auto& w : j["waves"]) {
        Wave wave;
        for (auto& a : w["actions"]) {
            WaveAction action;

            std::string typeStr = a["actionType"];
            if (typeStr == "SpawnEnemy") {
                action.actionType = ActionType::SpawnEnemy;
                action.enemyType= a["type"];
                action.enemySpawnId = a["id"];
            }
            else if (typeStr == "SpawnPlatform") {
                action.actionType = ActionType::SpawnPlatform;
                action.platformSpawnId = a["id"];
            }
            else if (typeStr == "RemovePlatform") {
                action.actionType = ActionType::RemovePlatform;
                action.platformSpawnId = a["id"];
            }
            else if (typeStr == "SpawnHookPoint") {
                action.actionType = ActionType::SpawnHookPoint;
                action.hookSpawnId = a["id"];
            }
            else if (typeStr == "RemoveHookPoint") {
                action.actionType = ActionType::RemoveHookPoint;
                action.hookSpawnId = a["id"];
            }
            else if (typeStr == "GrowGround") {
                action.actionType = ActionType::GrowGround;
                action.groundId = a["id"];
            }
            else if (typeStr == "DecreaseGround") {
                action.actionType = ActionType::DecreaseGround;
                action.groundId = a["id"];
            }
            else {
                // tipo desconhecido → ignora ou loga um erro
                continue;
            }

            action.delay = a["delay"];
            wave.actions.push_back(action);
        }
        mWaves.push_back(wave);
    }
    return true;
}

void WaveManager::Start() {
    mStarted = true;
    mCurrentWave = 0;
    mWaveTimer = 0.0f;
}

void WaveManager::Update(float deltaTime) {
    if (!mStarted || mCurrentWave < 0 || mCurrentWave >= (int)mWaves.size()) {
        return;
    }

    mWaveTimer += deltaTime;

    Wave& wave = mWaves[mCurrentWave];

    // Spawn inimigos conforme delay
    for (auto& a : wave.actions) {
        if (a.delay >= 0.0f && mWaveTimer >= a.delay) {
            Vector2 pos;
            HookPoint* hookPoint;
            Ground* g;
            DynamicGround* dynamicGround;
            switch (a.actionType) {
                case ActionType::SpawnEnemy:
                    SpawnEnemy(a);
                    break;

                case ActionType::SpawnPlatform:

                    break;

                case ActionType::RemovePlatform:

                    break;

                case ActionType::SpawnHookPoint:
                    pos = mGame->GetSpawnPointPosition(a.hookSpawnId);
                    hookPoint = new HookPoint(mGame);
                    hookPoint->SetPosition(pos);
                    a.hookPoint = hookPoint;
                    mHookPoints.emplace_back(hookPoint);
                    break;

                case ActionType::RemoveHookPoint:
                    pos = mGame->GetSpawnPointPosition(a.hookSpawnId);
                    for (auto it = mHookPoints.begin(); it != mHookPoints.end(); ++it) {
                        if ((*it)->GetPosition() == pos) {
                            auto hook = *it;
                            mHookPoints.erase(it);
                            hook->SetState(ActorState::Destroy);
                            break;;
                        }
                    }
                    break;

                case ActionType::GrowGround:
                    g = mGame->GetGroundById(a.groundId);
                    dynamicGround = dynamic_cast<DynamicGround*>(g);
                    if (dynamicGround) {
                        dynamicGround->SetIsGrowing(true);
                    }
                    break;

                case ActionType::DecreaseGround:
                    g = mGame->GetGroundById(a.groundId);
                    dynamicGround = dynamic_cast<DynamicGround*>(g);
                    if (dynamicGround) {
                        dynamicGround->SetIsDecreasing(true);
                    }
                    break;
            }

            // marca como feito
            a.delay = -1.0f;
        }
    }

    // Checar se todos inimigos morreram (depende do seu Game)
    if (AllEnemiesDefeated(wave)) {
        mCurrentWave++;
        mWaveTimer = 0.0f;
    }

    if (IsCompleted()) {
        mGame->RemoveWaveManager();
    }
}

bool WaveManager::IsCompleted() const {
    return mCurrentWave >= static_cast<int>(mWaves.size());
}

void WaveManager::SpawnEnemy(WaveAction& a) {
    // pega posição do spawn point no Game (carregado do Tiled)
    auto pos = mGame->GetSpawnPointPosition(a.enemySpawnId);

    if (a.enemyType == "Enemy Simple") {
        auto* enemySimple = new EnemySimple(mGame, 53, 45, 200, 30);
        enemySimple->SetPosition(pos);
        enemySimple->SetSpottedPlayer(true);
        a.enemy = enemySimple;
    }
    if (a.enemyType == "Flying Enemy") {
        auto* flyingEnemySimple = new FlyingEnemySimple(mGame, 70, 70, 250, 60);
        flyingEnemySimple->SetPosition(pos);
        flyingEnemySimple->SetSpottedPlayer(true);
        a.enemy = flyingEnemySimple;
    }
    if (a.enemyType == "FlyingShooterEnemy") {
        auto* flyingShooterEnemy = new FlyingShooterEnemy(mGame, 70, 70, 250, 60);
        flyingShooterEnemy->SetPosition(pos);
        flyingShooterEnemy->SetSpottedPlayer(true);
        a.enemy = flyingShooterEnemy;
    }
    if (a.enemyType == "Mantis") {
        auto* mantis = new Mantis(mGame, 120, 120, 250, 90);
        mantis->SetPosition(pos);
        mantis->SetSpottedPlayer(true);
        a.enemy = mantis;
    }
    if (a.enemyType == "FlyingGolem") {
        auto* flyingGolem = new FlyingGolem(mGame, 100, 100, 420, 130);
        flyingGolem->SetPosition(pos);
        flyingGolem->SetSpottedPlayer(true);
        a.enemy = flyingGolem;
    }
    if (a.enemyType == "DragonFly") {
        auto* dragonFly = new DragonFly(mGame, 130, 70, 1300, 120);
        dragonFly->SetPosition(pos);
        dragonFly->SetSpottedPlayer(true);
        a.enemy = dragonFly;
    }
    if (a.enemyType == "Fox") {
        auto* fox = new Fox(mGame, 100, 170, 300, 700);
        fox->SetPosition(pos);
        fox->SetSpottedPlayer(true);
        a.enemy = fox;
    }
}

bool WaveManager::AllEnemiesDefeated(Wave wave) {
    std::vector<class Enemy*> enemies = mGame->GetEnemies();

    for (auto& a : wave.actions) {
        if (a.actionType == ActionType::SpawnEnemy) {
            auto iter = std::find(enemies.begin(), enemies.end(), a.enemy);
            if (iter != enemies.end() || a.delay != -1) {
                return false;
            }
        }
    }
    return true;
}
