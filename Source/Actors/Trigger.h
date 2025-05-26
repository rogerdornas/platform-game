//
// Created by roger on 23/05/2025.
//

#pragma once
#include <string>
#include "Actor.h"

enum Target {
    camera,
    dynamicGround
};

enum Event {
    fixed,
    followPlayer,
    scrollRight,
    scrollUp,

    setIsGrowing
};

class Trigger : public Actor
{
public:
    Trigger(class Game *game, float width, float height);

    void SetTarget(std::string target);
    void SetEvent(std::string event);
    void SetGroundsIds(const std::vector<int>& ids) { mGroundsIds = ids; }

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

private:
    void CameraTrigger();
    void DynamicGroundTrigger();

    float mWidth;
    float mHeight;
    Target mTarget;
    Event mEvent;
    std::vector<int> mGroundsIds;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class AABBComponent *mAABBComponent;
};
