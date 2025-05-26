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
    DynamicGround(Game *game, float width, float height, bool isSpike = false, bool isMoving = false,
           float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);
    ~DynamicGround();

    void OnUpdate(float deltaTime) override;

    void SetIsGrowing(bool isGrowing) { mIsGrowing = isGrowing; }
    void SetMaxWidth(float width) { mMaxWidth = width * mGame->GetScale(); }
    void SetMaxHeight(float height) { mMaxHeight = height * mGame->GetScale(); }
    void SetGrowSpeed(Vector2 speed) { mGrowSpeed = speed * mGame->GetScale(); }
    void SetGrowDirection(GrowthDirection growDirection) { mGrowthDirection = growDirection; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mMaxWidth;
    float mMaxHeight;
    Vector2 mGrowSpeed;
    bool mIsGrowing;
    GrowthDirection mGrowthDirection;

    class DrawSpriteComponent *mDrawSpriteComponent;
};
