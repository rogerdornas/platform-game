//
// Created by roger on 28/05/2025.
//

#pragma once

#include "Actor.h"
#include "../Actors/Trigger.h"

class Lever : public Trigger
{
public:
    enum class LeverType {
        Lever,
        Crystal
    };

    Lever(class Game *game, float width = 64, float height = 64.0f, LeverType leverType = LeverType::Lever);

    void OnUpdate(float deltaTime) override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    void DynamicGroundTrigger() override;
    void EnemyTrigger() override;

    LeverType mLeverType;
    float mHealthPoints;
    bool mSwordHit;
    bool mActivate;
    float mActivatingDuration;
    float mActivatingTimer;

    class DrawSpriteComponent *mDrawSpriteComponent;
    class DrawAnimatedComponent *mDrawAnimatedComponent;
};
