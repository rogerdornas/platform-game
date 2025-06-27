//
// Created by roger on 25/05/2025.
//

#pragma once

#include "Actor.h"
#include "Ground.h"
#include "../Game.h"

enum class GrowthDirection
{
    Centered,
    Right,
    Left,
    Up,
    Down
};

class DynamicGround : public Ground
{
public:
    DynamicGround(Game* game, float width, float height, bool isSpike = false, bool isMoving = false,
                  float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);

    void OnUpdate(float deltaTime) override;

    void SetIsGrowing(bool isGrowing) { mIsGrowing = isGrowing; mIsDecreasing = !isGrowing; }
    void SetIsDecreasing(bool isDecreasing) { mIsDecreasing = isDecreasing; mIsGrowing = !isDecreasing; }
    void SetIsOscillating(bool isOscillating) { mIsOscillating = isOscillating; }
    void SetMaxWidth(float maxWidth) { mMaxWidth = maxWidth; }
    void SetMaxHeight(float maxHeight) { mMaxHeight = maxHeight; }
    void SetMinWidth(float minWidth) { mMinWidth = minWidth; }
    void SetMinHeight(float minHeight) { mMinHeight = minHeight; }
    void SetGrowSpeed(Vector2 speed) { mGrowSpeed = speed*  mGame->GetScale(); }
    void SetGrowDirection(GrowthDirection growDirection) { mGrowthDirection = growDirection; }

    void SetSprites() override;

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mMaxWidth;
    float mMaxHeight;
    float mMinWidth;
    float mMinHeight;
    Vector2 mGrowSpeed;
    bool mIsGrowing;
    bool mIsDecreasing;
    bool mIsOscillating;
    GrowthDirection mGrowthDirection;

    class DrawDynamicGroundSpritesComponent* mDrawDynamicGroundSpritesComponent;
};
