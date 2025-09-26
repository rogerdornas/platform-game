//
// Created by roger on 23/04/2025.
//

#pragma once

#include "Math.h"
#include "Game.h"

enum class CameraMode {
    Fixed,
    FollowPlayer,
    FollowPlayerHorizontally,
    FollowPlayerLimitRight,
    FollowPlayerLimitLeft,
    FollowPlayerLimitUp,
    FollowPlayerLimitDown,
    FollowPlayerLimited,
    FollowPlayerLimitRightHorizontally,
    FollowPlayerLimitLeftHorizontally,
    ScrollRight,
    ScrollUp,
    PanoramicCamera,
};

class Camera
{
public:
    Camera(class Game *game, Vector2 startPosition);

    void SetPosition(Vector2 pos) { mPos = pos; }
    Vector2 GetPosCamera() const { return mPos; }
    void SetFixedCameraPosition(Vector2 pos) { mFixedCameraPosition = pos; }

    void SetLimitMinCameraPosition(Vector2 pos);
    void SetLimitMaxCameraPosition(Vector2 pos);

    void Update(float deltaTime);

    void SetLookUp() { mLookUp = true; }
    void SetLookDown() { mLookDown = true; }

    void SetCameraVelocity(Vector2 velocity) { mCameraVelocity = velocity; }

    void StartCameraShake(float duration = 1.0f, float strength = 5.0f);
    void ChangeCameraMode(CameraMode cameraMode);
    void ChangeResolution(float oldScale, float newScale);

private:
    Vector2 Fixed(Vector2 pos);
    Vector2 FollowPlayer();
    Vector2 FollowPlayerHorizontally();
    Vector2 FollowPlayerLimitRight();
    Vector2 FollowPlayerLimitLeft();
    Vector2 FollowPlayerLimitUp();
    Vector2 FollowPlayerLimitDown();
    Vector2 FollowPlayerLimitRightHorizontally();
    Vector2 FollowPlayerLimitLeftHorizontally();
    Vector2 ScrollRight(float deltaTime, float speed);
    Vector2 ScrollUp(float deltaTime, float speed);
    Vector2 PanoramicCamera(float deltaTime);

    Vector2 mPos;
    class Game *mGame;
    float mCameraLerpSpeed;
    float mNormalSpeed;
    float mSlowTransitionSpeed;
    float mTransitionDuration;
    float mTransitionTimer;
    CameraMode mCameraMode;
    Vector2 mFixedCameraPosition;
    Vector2 mLimitMinCameraPosition;
    Vector2 mLimitMaxCameraPosition;
    Vector2 mCurrentLimitMinPosition;
    Vector2 mCurrentLimitMaxPosition;
    float mCurrentOffsetX;

    float mDistMove;

    bool mIsShaking;
    float mShakeDuration;
    float mShakeTimer;
    float mShakeStrength;

    float mCameraSpeed;
    Vector2 mCameraVelocity;

    bool mLookUp;
    bool mLookDown;
};
