//
// Created by roger on 23/05/2025.
//

#pragma once
#include <string>
#include "Actor.h"

enum Target {
    camera
};

enum Event {
    fixed,
    followPlayer,
    scrollRight,
    scrollUp
};

class Trigger : public Actor
{
public:
    Trigger(class Game *game, float width, float height);

    void SetTarget(std::string target);
    void SetEvent(std::string event);

    void OnUpdate(float deltaTime) override;
    void ChangeResolution(float oldScale, float newScale) override;

private:
    void CameraTrigger();

    float mWidth;
    float mHeight;
    Target mTarget;
    Event mEvent;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class AABBComponent *mAABBComponent;
};
