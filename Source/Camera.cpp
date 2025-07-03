//
// Created by roger on 23/04/2025.
//

#include "Camera.h"
#include "Random.h"

Camera::Camera(class Game* game, Vector2 startPosition)
    :mPos(startPosition)
    ,mGame(game)
    ,mCameraLerpSpeed(6.0f)
    ,mCameraMode(CameraMode::FollowPlayer)
    ,mFixedCameraPosition(Vector2::Zero)
    ,mDistMove(400 * mGame->GetScale())
    ,mIsShaking(false)
    ,mShakeDuration(1.0f)
    ,mShakeTimer(0.0f)
    ,mShakeStrength(5.0f * mGame->GetScale())
    ,mCameraSpeed(500.0f * mGame->GetScale())
    ,mCameraVelocity(Vector2::Zero)
    ,mLookUp(false)
    ,mLookDown(false)
{
}

void Camera::StartCameraShake(float duration, float strength) {
    mIsShaking = true;
    mShakeTimer = 0.0f;
    mShakeDuration = duration;
    mShakeStrength = strength;
}

void Camera::Update(float deltaTime) {
    Vector2 targetPosition = Vector2::Zero;
    switch (mCameraMode) {
        case CameraMode::Fixed:
            targetPosition = Fixed(Vector2(mFixedCameraPosition));
            break;

        case CameraMode::FollowPlayer:
            targetPosition = FollowPlayer();
            break;

        case CameraMode::FollowPlayerHorizontally:
            targetPosition = FollowPlayerHorizontally();
            break;

        case CameraMode::FollowPlayerLimitRight:
            targetPosition = FollowPlayerLimitRight();
            break;

        case CameraMode::FollowPlayerLimitLeft:
            targetPosition = FollowPlayerLimitLeft();
            break;

        case CameraMode::FollowPlayerLimitRightHorizontally:
            targetPosition = FollowPlayerLimitRightHorizontally();
            break;

        case CameraMode::ScrollRight:
            targetPosition = ScrollRight(deltaTime, mCameraSpeed);
            break;

        case CameraMode::ScrollUp:
            targetPosition = ScrollUp(deltaTime, -mCameraSpeed / 2);
            break;

        case CameraMode::PanoramicCamera:
            targetPosition = PanoramicCamera(deltaTime);
            break;
    }

    // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
    if (mLookUp) {
        targetPosition.y -= mDistMove;
    }
    else if (mLookDown) {
        targetPosition.y += mDistMove;
    }

    // Interpola camera se estiver seguindo o player
    if (mCameraMode == CameraMode::FollowPlayer ||
        mCameraMode == CameraMode::FollowPlayerHorizontally ||
        mCameraMode == CameraMode::FollowPlayerLimitRight ||
        mCameraMode == CameraMode::FollowPlayerLimitLeft ||
        mCameraMode == CameraMode::FollowPlayerLimitRightHorizontally )
    {
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
                       int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::ScrollRight) {
        mPos = Vector2(targetPosition.x,
                    int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::ScrollUp) {
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
                        targetPosition.y);
    }
    else if (mCameraMode == CameraMode::Fixed) {
        // mPos = targetPosition;
        mPos = Vector2(int(mPos.x + (targetPosition.x - mPos.x) * mCameraLerpSpeed * deltaTime),
               int(mPos.y + (targetPosition.y - mPos.y) * mCameraLerpSpeed * deltaTime));
    }
    else if (mCameraMode == CameraMode::PanoramicCamera) {
        mPos = targetPosition;
    }

    // Camera Shake
    if (mShakeTimer < mShakeDuration) {
        mShakeTimer += deltaTime;
    }

    float shakeOffsetX = 0;
    float shakeOffsetY = 0;

    if (mIsShaking) {
        shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
        shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;

        if (mShakeTimer >= mShakeDuration) {
            mIsShaking = false;
            shakeOffsetX = 0;
            shakeOffsetY = 0;
        }
    }
    mPos.x += shakeOffsetX * mCameraLerpSpeed * deltaTime;
    mPos.y += shakeOffsetY * mCameraLerpSpeed * deltaTime;

    // Reset flags para o próximo frame
    mLookUp = false;
    mLookDown = false;
}

Vector2 Camera::Fixed(Vector2 pos) {
    return pos;
}

Vector2 Camera::FollowPlayer() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetLogicalWindowWidth() / 2,
                      playerPos.y - mGame->GetLogicalWindowHeight() / 2);
    return targetPos;
}

Vector2 Camera::FollowPlayerHorizontally() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetLogicalWindowWidth() / 2,
                      mFixedCameraPosition.y);
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitRight() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetLogicalWindowWidth() / 2,
                      playerPos.y - mGame->GetLogicalWindowHeight() / 2);

    if (targetPos.x + mGame->GetLogicalWindowWidth() > mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x - mGame->GetLogicalWindowWidth();
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitLeft() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetLogicalWindowWidth() / 2,
                      playerPos.y - mGame->GetLogicalWindowHeight() / 2);

    if (targetPos.x < mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x;
    }
    return targetPos;
}

Vector2 Camera::FollowPlayerLimitRightHorizontally() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetLogicalWindowWidth() / 2,
                      mFixedCameraPosition.y);

    if (targetPos.x + mGame->GetLogicalWindowWidth() > mFixedCameraPosition.x) {
        targetPos.x = mFixedCameraPosition.x - mGame->GetLogicalWindowWidth();
    }
    return targetPos;
}

Vector2 Camera::ScrollRight(float deltaTime, float speed) {
    Vector2 targetPos = GetPosCamera();
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        targetPos.x = mPos.x + speed * deltaTime;
        targetPos.y = playerPos.y - mGame->GetLogicalWindowHeight() / 2;

        if (playerPos.x < targetPos.x - 50 * mGame->GetScale()) {
            mGame->SetResetLevel();
            mGame->GetAudio()->StopAllSounds();
            mGame->GetPlayer()->SetState(ActorState::Paused);
            mFixedCameraPosition = mPos - Vector2(mGame->GetLogicalWindowWidth() / 2, 0);
            mCameraMode = CameraMode::Fixed;
        }
    }
    return targetPos;
}

Vector2 Camera::ScrollUp(float deltaTime, float speed) {
    Vector2 targetPos = GetPosCamera();
    if (mGame->GetGamePlayState() == Game::GamePlayState::Playing) {
        Vector2 playerPos = mGame->GetPlayer()->GetPosition();
        targetPos.x = playerPos.x - mGame->GetLogicalWindowWidth() / 2;
        targetPos.y = mPos.y + speed * deltaTime;


        if (playerPos.y > targetPos.y + mGame->GetLogicalWindowHeight() + 50 * mGame->GetScale()) {
            mGame->SetResetLevel();
            mGame->GetAudio()->StopAllSounds();
            mGame->GetPlayer()->SetState(ActorState::Paused);
            mFixedCameraPosition = mPos + Vector2(0, mGame->GetLogicalWindowHeight() / 2);
            mCameraMode = CameraMode::Fixed;
        }
    }
    return targetPos;
}

Vector2 Camera::PanoramicCamera(float deltaTime) {
    Vector2 targetPos = GetPosCamera();
    targetPos.x = mPos.x + mCameraVelocity.x * deltaTime;
    targetPos.y = mPos.y + mCameraVelocity.y * deltaTime;
    return targetPos;
}


// void Camera::Update(float deltaTime) {
//     Vector2 playerPos = mGame->GetPlayer()->GetPosition();
//     Vector2 basePos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight()/2);
//
//     // Define o alvo do deslocamento (offset)
//     Vector2 targetOffset(0.0f, 0.0f);
//     if (mLookUp) {
//         mTimerToStartLooking += deltaTime;
//         if (mTimerToStartLooking >= mLookDelay) {
//             targetOffset.y = -mDistMove;
//         }
//     } else if (mLookDown) {
//         mTimerToStartLooking += deltaTime;
//         if (mTimerToStartLooking >= mLookDelay) {
//             targetOffset.y = mDistMove;
//         }
//     }
//     if (!mLookUp && !mLookDown) {
//         mTimerToStartLooking = 0.0f;
//     }
//
//     // Sempre interpola o deslocamento suavemente
//     mOffset = Vector2(
//         mOffset.x + (targetOffset.x - mOffset.x) * mCameraLerpSpeed * deltaTime,
//         mOffset.y + (targetOffset.y - mOffset.y) * mCameraLerpSpeed * deltaTime
//     );
//
//     // Atualiza a posição final da câmera: posição do player + deslocamento suave
//     mPos = basePos + mOffset;
//
//     // Reset flags
//     mLookUp = false;
//     mLookDown = false;
//
//     // Trava a câmera para não descer demais
//     // if (mPos.y > mGame->GetWindowHeight()) {
//     //     mPos.y = mGame->GetWindowHeight();
//     // }
// }

// void Camera::Update(float deltaTime)
// {
//     // Shake
//     if (mShakeTimer < mShakeDuration)
//         mShakeTimer += deltaTime;
//
//     float shakeOffsetX = 0;
//     float shakeOffsetY = 0;
//
//     if (mIsShaking)
//     {
//         shakeOffsetX = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
//         shakeOffsetY = Random::GetFloat() * (2 * mShakeStrength + 1) - mShakeStrength;
//
//         if (mShakeTimer >= mShakeDuration)
//         {
//             mIsShaking = false;
//             shakeOffsetX = 0;
//             shakeOffsetY = 0;
//         }
//     }
//
//     Vector2 playerPos = mGame->GetPlayer()->GetPosition();
//     // Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - 2 * mGame->GetWindowHeight() / 3); // Centro da camera a 2/3 do topo
//     // Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight() / 2); // Centro da camera no centro da altura da tela
//     Vector2 targetPos(playerPos.x - mGame->GetWindowWidth() / 2 + shakeOffsetX,
//                       playerPos.y - mGame->GetWindowHeight() / 2 + shakeOffsetY);
//     // Centro da camera no centro da altura da tela
//
//     // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
//     if (mLookUp)
//     {
//         mTimerToStartLooking += deltaTime;
//         if (mTimerToStartLooking >= mLookDelay)
//             targetPos.y -= mDistMove;
//
//     }
//     else if (mLookDown)
//     {
//         mTimerToStartLooking += deltaTime;
//         if (mTimerToStartLooking >= mLookDelay)
//             targetPos.y += mDistMove;
//
//     }
//
//     if (!mLookUp && !mLookDown)
//         mTimerToStartLooking = 0.0f;
//
//     // Trava a camera na arena do boss
//     if (playerPos.x > 22080 * mGame->GetScale() && playerPos.x < 24000 * mGame->GetScale())
//         targetPos = Vector2(22080 * mGame->GetScale() + shakeOffsetX, 15420 * mGame->GetScale() + shakeOffsetY);
//
//     // Sempre interpola a posição atual da câmera para a posição-alvo
//     mPos = Vector2(
//         int(mPos.x + (targetPos.x - mPos.x) * mCameraLerpSpeed * deltaTime),
//         (mPos.y + (targetPos.y - mPos.y) * mCameraLerpSpeed * deltaTime)
//     );
//
//     // mPos = Vector2::Lerp(mPos, targetPos, mCameraLerpSpeed * deltaTime);
//
//     // Reset flags para o próximo frame
//     mLookUp = false;
//     mLookDown = false;
//
//     // Trava a câmera abaixo de certo valor
//     // if (mPos.y > mGame->GetWindowHeight()) {
//     //     mPos.y = mGame->GetWindowHeight();
//     // }
// }


// void Camera::Update(float deltaTime) {
//     Player* player = mGame->GetPlayer();
//     float speed = 500 * mGame->GetScale();
//
//     mPos.x += speed * deltaTime;
//     // float targetPosY = player->GetPosition().y - 2 * (mGame->GetWindowHeight() / 3);
//     float targetPosY = player->GetPosition().y - mGame->GetWindowHeight() / 2;
//     mPos.y = int(mPos.y + (targetPosY - mPos.y) * mCameraLerpSpeed * deltaTime);
//
//
//     if (player->GetPosition().x < mPos.x - 50) {
//         mGame->mResetLevel = true;
//         // player->SetPosition(player->GetStartingPosition());
//         // mPos = player->GetStartingPosition();
//         // mPos = Vector2(player->GetStartingPosition().x - mGame->GetWindowWidth()/2, player->GetStartingPosition().y - mGame->GetWindowHeight()/2);
//
//     }
// }

void Camera::ChangeResolution(float oldScale, float newScale) {
    mPos.x = mPos.x / oldScale * newScale;
    mPos.y = mPos.y / oldScale * newScale;
    mDistMove = mDistMove / oldScale * newScale;
    mShakeStrength = mShakeStrength / oldScale * newScale;
    mCameraSpeed = mCameraSpeed / oldScale * newScale;
    mFixedCameraPosition.x = mFixedCameraPosition.x / oldScale * newScale;
    mFixedCameraPosition.y = mFixedCameraPosition.y / oldScale * newScale;
}
