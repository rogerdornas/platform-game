//
// Created by roger on 22/08/2025.
//

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "Actors/Enemy.h"
#include "Actors/HookPoint.h"

enum class ActionType {
    SpawnEnemy,
    SpawnPlatform,
    SpawnHookPoint,
    RemovePlatform,
    RemoveHookPoint
};

struct WaveAction {
    ActionType actionType;
    float delay;             // segundos após início da wave

    std::string enemyType;
    std::string enemySpawnId;
    std::string hookSpawnId;
    std::string platformSpawnId;
    Enemy* enemy;
    HookPoint* hookPoint;
};

struct Wave {
    std::vector<WaveAction> actions;
};



class WaveManager
{
public:
    WaveManager(class Game* game);

    // carregar waves de um json
    bool LoadFromJson(const std::string& filePath);

    // atualizar a lógica das waves
    void Update(float deltaTime);

    // iniciar as waves
    void Start();

    // checar se terminou todas as waves
    bool IsCompleted() const;

private:
    class Game* mGame;

    std::vector<Wave> mWaves;
    int mCurrentWave;
    float mWaveTimer; // tempo desde início da wave

    bool mStarted;

    std::vector<class HookPoint*> mHookPoints;

    // auxiliar: spawn inimigo
    void SpawnEnemy(WaveAction& e);

    bool AllEnemiesDefeated(Wave wave);
};

