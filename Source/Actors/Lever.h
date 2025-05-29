//
// Created by roger on 28/05/2025.
//

#pragma once
#include <string>
#include "Actor.h"
#include "../Actors/Trigger.h"

class Lever : public Trigger
{
public:
    Lever(class Game *game);

    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void DynamicGroundTrigger();

    bool mActivate;
    float mActivatingDuration;
    float mActivatingTimer;

    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;
};
