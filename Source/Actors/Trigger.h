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
        Renderer,
        Enemy,
        Dialogue,
        Cutscene,
        Player,
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
        ChangeWorldState,
        ChangeZoom,

        ChangeAmbientLight,

        SpotPlayer,
        GolemVulnerable,

        StartDialogue,

        StartCutscene,

        InvertControls,
        RevertControls
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
    void SetWorldState(std::string worldState) { mWorldState = worldState; }
    void SetWorldStateFlag(bool worldStateFlag) { mWorldStateFlag = worldStateFlag; }
    void SetTargetZoom(float targetZoom) { mTargetZoom = targetZoom; }
    void SetZoomSpeed(float zoomSpeed) { mZoomSpeed = zoomSpeed; }
    void SetAmbientColor(Vector3 ambientColor) { mAmbientColor = ambientColor; }
    void SetAmbientIntensity(float ambientIntensity) { mAmbientIntensity = ambientIntensity; }
    void SetDialoguePath(const std::string &dialoguePath) { mDialoguePath = dialoguePath; }
    void SetCutsceneId(const std::string &cutsceneId) { mCutsceneId = cutsceneId; }

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

protected:
    void CameraTrigger();
    void DynamicGroundTrigger();
    void GroundTrigger();
    void GameTrigger();
    void RendererTrigger();
    virtual void EnemyTrigger();
    void DialogueTrigger();
    void CutsceneTrigger();
    void PlayerTrigger();

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
    std::string mWorldState;
    bool mWorldStateFlag;
    float mTargetZoom;
    float mZoomSpeed;

    Vector3 mAmbientColor;
    float mAmbientIntensity;

    std::string mDialoguePath;
    std::string mCutsceneId;

    class RectComponent* mRectComponent;
    class ColliderComponent *mAABBComponent;
};
