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
        Nothing
    };

    enum class Event {
        Fixed,
        FollowPlayer,
        FollowPlayerHorizontally,
        FollowPlayerLimitRight,
        ScrollRight,
        ScrollUp,

        SetIsGrowing,
        SetIsDecreasing,
        SetIsDecreasingAfterKillingEnemies,

        ChangeScene,

        SpotPlayer,

        SetIsMoving
    };

    Trigger(class Game *game, float width, float height);

    void SetTarget(std::string target);
    void SetEvent(std::string event);
    void SetGroundsIds(const std::vector<int>& ids) { mGroundsIds = ids; }
    void SetEnemiesIds(const std::vector<int>& ids) { mEnemiesIds = ids; }
    void SetFixedCameraPosition(Vector2 pos) { mFixedCameraPosition = pos * mGame->GetScale(); }
    void SetScene(std::string scene);

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

protected:
    void CameraTrigger();
    virtual void DynamicGroundTrigger();
    void GroundTrigger();
    void GameTrigger();
    void EnemyTrigger();

    float mWidth;
    float mHeight;
    Target mTarget;
    Event mEvent;
    std::vector<int> mGroundsIds;
    std::vector<int> mEnemiesIds;
    Vector2 mFixedCameraPosition;

    Game::GameScene mScene;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class AABBComponent *mAABBComponent;
};
