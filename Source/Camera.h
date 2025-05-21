//
// Created by roger on 23/04/2025.
//

#pragma once
#include "Math.h"
#include "Game.h"

class Camera
{
public:
    Camera(class Game *game, Vector2 startPosition);

    void SetPosition(Vector2 pos) { mPos = pos; }
    Vector2 GetPosCamera() { return mPos; }
    void Update(float deltaTime);

    void StartCameraShake(float duration = 1.0f, float strength = 5.0f);

    bool mLookUp;
    bool mLookDown;

private:
    Vector2 mPos;
    class Game *mGame;
    const float mCameraLerpSpeed = 6.0f;

    Vector2 mOffset;
    float mDistMove;
    float mTimerToStartLooking;
    float mLookDelay;

    bool mIsShaking;
    float mShakeDuration;
    float mShakeTimer;
    float mShakeStrength;
};
