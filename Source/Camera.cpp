//
// Created by roger on 23/04/2025.
//

#include "Camera.h"

#include "Random.h"

Camera::Camera(class Game *game, Vector2 startPosition)
    :mGame(game)
    ,mPos(startPosition)
{
    mLookUp = false;
    mLookDown = false;
    mOffset = Vector2::Zero;
    mDistMove = 200;
    mTimerToStartLooking = 0.0f;
    mLookDelay = 1.2f;
    mIsShaking = false;
    mShakeDuration = 1.0f;
    mShakeTimer = 0.0f;
    mShakeStrength = 5;
}

void Camera::StartCameraShake(float duration, float strength) {
    mIsShaking = true;
    mShakeTimer = 0.0f;
    mShakeDuration = duration;
    mShakeStrength = strength;
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

void Camera::Update(float deltaTime) {

    // Shake
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

    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    // Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - 2 * mGame->GetWindowHeight() / 3); // Centro da camera a 2/3 do topo
    // Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight() / 2); // Centro da camera no centro da altura da tela
    Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2 + shakeOffsetX, playerPos.y - mGame->GetWindowHeight() / 2 + shakeOffsetY); // Centro da camera no centro da altura da tela

    // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
    if (mLookUp) {
        mTimerToStartLooking += deltaTime;
        if (mTimerToStartLooking >= mLookDelay) {
            targetPos.y -= mDistMove;
        }
    }
    else if (mLookDown) {
        mTimerToStartLooking += deltaTime;
        if (mTimerToStartLooking >= mLookDelay) {
            targetPos.y += mDistMove;
        }
    }

    if (!mLookUp && !mLookDown) {
        mTimerToStartLooking = 0.0f;
    }

    // Trava a camera na arena do boss
    if (playerPos.x > 22080 * mGame->GetScale() && playerPos.x < 24000 * mGame->GetScale()) {
        targetPos = Vector2(22080 * mGame->GetScale() + shakeOffsetX, 15420 * mGame->GetScale() + shakeOffsetY);
    }

    // Sempre interpola a posição atual da câmera para a posição-alvo
    mPos = Vector2(
        int(mPos.x + (targetPos.x - mPos.x) * mCameraLerpSpeed * deltaTime),
        (mPos.y + (targetPos.y - mPos.y) * mCameraLerpSpeed * deltaTime)
    );

    // mPos = Vector2::Lerp(mPos, targetPos, mCameraLerpSpeed * deltaTime);

    // Reset flags para o próximo frame
    mLookUp = false;
    mLookDown = false;

    // Trava a câmera abaixo de certo valor
    // if (mPos.y > mGame->GetWindowHeight()) {
    //     mPos.y = mGame->GetWindowHeight();
    // }
}


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
//         player->SetPosition(player->GetStartingPosition());
//         // mPos = player->GetStartingPosition();
//         mPos = Vector2(player->GetStartingPosition().x - mGame->GetWindowWidth()/2, player->GetStartingPosition().y - mGame->GetWindowHeight()/2);
//
//     }
// }
