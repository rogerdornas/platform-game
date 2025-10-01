//
// Created by roger on 23/05/2025.
//

#pragma once

#include <string>
#include "Actor.h"
#include "../Game.h"

class Trigger : public Actor
{
public:
    enum class Target {
        Camera,
        DynamicGround,
        Ground,
        Game,
        Enemy,
        Dialogue,
        Cutscene,
        Nothing
    };

    enum class Event {
        Fixed,
        FollowPlayer,
        FollowPlayerHorizontally,
        FollowPlayerLimitRight,
        FollowPlayerLimitLeft,
        FollowPlayerLimitUp,
        FollowPlayerLimitDown,
        FollowPlayerLimited,
        FollowPlayerLimitRightHorizontally,
        FollowPlayerLimitLeftHorizontally,
        ScrollRight,
        ScrollUp,

        SetIsGrowing,
        SetIsDecreasing,
        SetIsDecreasingAfterKillingEnemies,

        SetIsMoving,

        ChangeScene,
        StartArena,

        SpotPlayer,
        GolemVulnerable,

        StartDialogue,

        StartCutscene
    };

    Trigger(class Game *game, float width, float height);

    void SetTarget(std::string target);
    void SetEvent(std::string event);
    void SetDestroy(bool destroy) { mDestroy = destroy; }
    void SetGroundsIds(const std::vector<int>& ids) { mGroundsIds = ids; }
    void SetEnemiesIds(const std::vector<int>& ids) { mEnemiesIds = ids; }
    void SetFixedCameraPosition(Vector2 pos) { mFixedCameraPosition = pos * mGame->GetScale(); }
    void SetLimitMinCameraPosition(Vector2 pos) { mLimitMinCameraPosition = pos * mGame->GetScale(); }
    void SetLimitMaxCameraPosition(Vector2 pos) { mLimitMaxCameraPosition = pos * mGame->GetScale(); }
    void SetScene(std::string scene);
    void SetPlayerStartPositionId(int id) { mPlayerStartPositionId = id; }
    void SetWavesPath(const std::string &wavesPath);
    void SetDialoguePath(const std::string &dialoguePath) { mDialoguePath = dialoguePath; }
    void SetCutsceneId(const std::string &cutsceneId) { mCutsceneId = cutsceneId; }

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

protected:
    void CameraTrigger();
    void DynamicGroundTrigger();
    void GroundTrigger();
    void GameTrigger();
    virtual void EnemyTrigger();
    void DialogueTrigger();
    void CutsceneTrigger();

    float mWidth;
    float mHeight;
    Target mTarget;
    Event mEvent;
    bool mDestroy;
    std::vector<int> mGroundsIds;
    std::vector<int> mEnemiesIds;
    Vector2 mFixedCameraPosition;
    Vector2 mLimitMinCameraPosition;
    Vector2 mLimitMaxCameraPosition;

    Game::GameScene mScene;
    int mPlayerStartPositionId;
    std::string mWavesPath;

    std::string mDialoguePath;
    std::string mCutsceneId;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class ColliderComponent *mAABBComponent;
};
