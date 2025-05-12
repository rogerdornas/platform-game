//
// Created by roger on 23/04/2025.
//

#pragma once
#include "Math.h"
#include "Game.h"

class Camera {
public:
    Camera(class Game* game, Vector2 startPosition);

    Vector2 GetPosCamera() { return mPos; }
    void Update(float deltaTime);

    bool mLookUp;
    bool mLookDown;

private:
    Vector2 mPos;
    class Game* mGame;
    const float mCameraLerpSpeed = 6.0f;
    Vector2 mOffset;
    float mDistMove;
    float mTimerToStartLooking;
    float mLookDelay;
};


