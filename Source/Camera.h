//
// Created by roger on 23/04/2025.
//

#pragma once
#include "Math.h"
#include "Game.h"

enum CameraMode {
    Fixed,
    FollowPlayer,
    FollowPlayerHorizontally,
    ScrollRight,
    ScrollUp
};

class Camera
{
public:
    Camera(class Game *game, Vector2 startPosition);

    void SetPosition(Vector2 pos) { mPos = pos; }
    Vector2 GetPosCamera() { return mPos; }
    void SetFixedCameraPosition(Vector2 pos) { mFixedCameraPosition = pos; }

    void Update(float deltaTime);

    void StartCameraShake(float duration = 1.0f, float strength = 5.0f);
    void ChangeCameraMode(CameraMode cameraMode) { mCameraMode = cameraMode; }
    void ChangeResolution(float oldScale, float newScale);

    bool mLookUp;
    bool mLookDown;

private:
    Vector2 Fixed(Vector2 pos);
    Vector2 FollowPlayer();
    Vector2 FollowPlayerHorizontally();
    Vector2 ScrollRight(float deltaTime, float speed);
    Vector2 ScrollUp(float deltaTime, float speed);

    Vector2 mPos;
    class Game *mGame;
    float mCameraLerpSpeed = 6.0f;
    CameraMode mCameraMode;
    Vector2 mFixedCameraPosition;

    Vector2 mOffset;
    float mDistMove;

    bool mIsShaking;
    float mShakeDuration;
    float mShakeTimer;
    float mShakeStrength;

    float mCameraSpeed;
};
