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
    GrowGround,
    RemovePlatform,
    RemoveHookPoint,
    DecreaseGround,
    CreateSpawner
};

enum class ConditionType {
    None,
    EnemyHealthBelow,
    EnemyDefeated,
};

struct WaveAction {
    ActionType actionType;
    float delay;             // segundos após início da wave
    bool executed = false;

    ConditionType condition = ConditionType::None;
    float conditionValue = 0.0f;

    std::string enemyType;
    std::string enemySpawnId;
    std::string hookSpawnId;
    std::string platformSpawnId;
    std::string enemyId;
    int groundId;
    Vector2 arenaMinPos;
    Vector2 arenaMaxPos;
    Enemy* enemy = nullptr;
    HookPoint* hookPoint = nullptr;
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
    bool CheckCondition(WaveAction& a);

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
    void CreateSpawner(WaveAction& a);

    bool AllEnemiesDefeated(Wave wave);
};

