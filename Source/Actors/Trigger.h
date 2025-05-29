//
// Created by roger on 23/05/2025.
//

#pragma once
#include <string>
#include "Actor.h"
#include "../Game.h"

enum Target {
    camera,
    dynamicGround,
    ground
};

enum Event {
    fixed,
    followPlayer,
    scrollRight,
    scrollUp,

    setIsGrowing,
    setIsDecreasing,

    setIsMoving
};

class Trigger : public Actor
{
public:
    Trigger(class Game *game, float width, float height);

    void SetTarget(std::string target);
    void SetEvent(std::string event);
    void SetGroundsIds(const std::vector<int>& ids) { mGroundsIds = ids; }
    void SetFixedCameraPosition(Vector2 pos) { mFixedCameraPosition = pos * mGame->GetScale(); }

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

protected:
    void CameraTrigger();
    void DynamicGroundTrigger();
    void GroundTrigger();

    float mWidth;
    float mHeight;
    Target mTarget;
    Event mEvent;
    std::vector<int> mGroundsIds;
    Vector2 mFixedCameraPosition;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class AABBComponent *mAABBComponent;
};
