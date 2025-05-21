//
// Created by roger on 22/04/2025.
//

#pragma once
#include "Actor.h"

class Ground : public Actor
{
public:
    Ground(Game *game, float width, float height, bool isSpike = false, bool isMoving = false,
           float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);
    ~Ground();

    void OnUpdate(float deltaTime) override;

    bool GetIsSpike() const { return mIsSpike; }
    bool GetIsMoving() const { return mIsMoving; }

    float GetHeight() const { return mHeight; }
    float GetWidth() const { return mWidth; }

    void SetSprites();

    void SetStartingPosition(Vector2 pos) { mStartingPosition = pos; }

    void ChangeResolution(float oldScale, float newScale) override;

private:
    float mHeight;
    float mWidth;
    bool mIsSpike;
    bool mIsMoving;
    float mMovingTimer;
    float mMovingDuration;
    Vector2 mStartingPosition;

    class DrawPolygonComponent *mDrawPolygonComponent;
    class DrawSpriteComponent *mDrawSpriteComponent;

    class DrawGroundSpritesComponent *mDrawGroundSpritesComponent;

    class RigidBodyComponent *mRigidBodyComponent;
    class AABBComponent *mAABBComponent;
};
