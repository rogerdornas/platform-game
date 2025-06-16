//
// Created by roger on 22/04/2025.
//

#pragma once

#include "Actor.h"

class Ground : public Actor
{
public:
    Ground(Game* game, float width, float height, bool isSpike = false, bool isMoving = false,
           float movingDuration = 0.0f, Vector2 velocity = Vector2::Zero);
    ~Ground() override;

    void OnUpdate(float deltaTime) override;

    bool GetIsSpike() const { return mIsSpike; }
    bool GetIsMoving() const { return mIsMoving; }
    void SetIsMoving(bool isMoving);

    float GetWidth() override { return mWidth; }
    float GetHeight() override { return mHeight; }

    void SetRespawPosition(Vector2 pos) { mRespawnPosition = pos; }
    Vector2 GetRespawPosition() { return mRespawnPosition; }

    virtual void SetSprites();

    void SetStartingPosition(Vector2 pos) { mStartingPosition = pos; }
    Vector2 GetStartingPosition() { return mStartingPosition; }

    void SetId(int id) { mId = id; }
    int GetId() { return mId; }

    void ChangeResolution(float oldScale, float newScale) override;

protected:
    int mId;
    float mHeight;
    float mWidth;
    bool mIsSpike;
    bool mIsMoving;
    float mMovingTimer;
    float mMovingDuration;
    Vector2 mStartingPosition;
    Vector2 mRespawnPosition;
    Vector2 mVelocity;

    class DrawPolygonComponent* mDrawPolygonComponent;
    class DrawGroundSpritesComponent* mDrawGroundSpritesComponent;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBComponent* mAABBComponent;
};
