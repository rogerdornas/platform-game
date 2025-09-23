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
#include "Actors/FlyingSpawnerEnemy.h"
#include "Actors/Fox.h"
#include "Actors/Frog.h"
#include "Actors/Golem.h"
#include "Actors/HookEnemy.h"
#include "Actors/LittleBat.h"
#include "Actors/Mantis.h"
#include "Actors/Spawner.h"

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
                if (a.contains("arenaMinPos")) {
                    action.arenaMinPos.x = a["arenaMinPos"][0];
                    action.arenaMinPos.y = a["arenaMinPos"][1];
                }
                if (a.contains("arenaMaxPos")) {
                    action.arenaMaxPos.x = a["arenaMaxPos"][0];
                    action.arenaMaxPos.y = a["arenaMaxPos"][1];
                }
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

            if (action.actionType == ActionType::SpawnEnemy) {
                WaveAction spawner;
                spawner.actionType = ActionType::CreateSpawner;
                spawner.enemyType = action.enemyType;
                spawner.delay = action.delay - 1.7f;
                spawner.enemySpawnId = action.enemySpawnId;
                wave.actions.push_back(spawner);
            }
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

                case ActionType::CreateSpawner:
                    CreateSpawner(a);
                    break;
            }

            // marca como feito
            a.delay = -1.0f;
        }
    }

    // Checar se a wave acabou
    if (AllEnemiesDefeated(wave)) {
        bool waveFinished = true;
        for (auto& action : wave.actions) {
            if (action.delay != -1.0f) {
                waveFinished = false;
            }
        }
        if (waveFinished) {
            mCurrentWave++;
            mWaveTimer = 0.0f;
        }
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
        auto* enemySimple = new EnemySimple(mGame);
        enemySimple->SetPosition(pos);
        enemySimple->SetSpottedPlayer(true);
        a.enemy = enemySimple;
    }
    if (a.enemyType == "Flying Enemy") {
        auto* flyingEnemySimple = new FlyingEnemySimple(mGame);
        flyingEnemySimple->SetPosition(pos);
        flyingEnemySimple->SetSpottedPlayer(true);
        a.enemy = flyingEnemySimple;
    }
    if (a.enemyType == "FlyingShooterEnemy") {
        auto* flyingShooterEnemy = new FlyingShooterEnemy(mGame);
        flyingShooterEnemy->SetPosition(pos);
        flyingShooterEnemy->SetSpottedPlayer(true);
        a.enemy = flyingShooterEnemy;
    }
    if (a.enemyType == "Mantis") {
        auto* mantis = new Mantis(mGame);
        mantis->SetPosition(pos);
        mantis->SetSpottedPlayer(true);
        a.enemy = mantis;
    }
    if (a.enemyType == "FlyingGolem") {
        auto* flyingGolem = new FlyingGolem(mGame);
        flyingGolem->SetPosition(pos);
        flyingGolem->SetSpottedPlayer(true);
        a.enemy = flyingGolem;
    }
    if (a.enemyType == "DragonFly") {
        auto* dragonFly = new DragonFly(mGame);
        dragonFly->SetPosition(pos);
        dragonFly->SetSpottedPlayer(true);
        a.enemy = dragonFly;
    }
    if (a.enemyType == "Fox") {
        auto* fox = new Fox(mGame);
        fox->SetPosition(pos);
        fox->SetSpottedPlayer(true);
        a.enemy = fox;
    }
    if (a.enemyType == "Golem") {
        auto* golem = new Golem(mGame);
        golem->SetPosition(pos);
        golem->SetSpottedPlayer(true);
        golem->SetArenaMinPos(Vector2(a.arenaMinPos.x, a.arenaMinPos.y));
        golem->SetArenaMaxPos(Vector2(a.arenaMaxPos.x, a.arenaMaxPos.y));
        a.enemy = golem;
    }
    if (a.enemyType == "Frog") {
        auto* frog = new Frog(mGame);
        frog->SetPosition(pos);
        frog->SetSpottedPlayer(true);
        frog->SetArenaMinPos(Vector2(a.arenaMinPos.x, a.arenaMinPos.y));
        frog->SetArenaMaxPos(Vector2(a.arenaMaxPos.x, a.arenaMaxPos.y));
        a.enemy = frog;
    }
    if (a.enemyType == "HookEnemy") {
        auto* hookEnemy = new HookEnemy(mGame);
        hookEnemy->SetPosition(pos);
        hookEnemy->SetSpottedPlayer(true);
        a.enemy = hookEnemy;
    }
    if (a.enemyType == "FlyingSpawnerEnemy") {
        auto* flyingSpawnerEnemy = new FlyingSpawnerEnemy(mGame);
        flyingSpawnerEnemy->SetPosition(pos);
        flyingSpawnerEnemy->SetSpottedPlayer(true);
        a.enemy = flyingSpawnerEnemy;
    }
    if (a.enemyType == "LittleBat") {
        auto* littleBat = new LittleBat(mGame);
        littleBat->SetPosition(pos);
        littleBat->SetSpottedPlayer(true);
        a.enemy = littleBat;
    }
}

void WaveManager::CreateSpawner(WaveAction &a) {
    auto* spawner = new Spawner(mGame);
    auto pos = mGame->GetSpawnPointPosition(a.enemySpawnId);
    spawner->SetPosition(pos);
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
