//
// Created by roger on 23/04/2025.
//

#include "Camera.h"

Camera::Camera(class Game *game)
    :mGame(game)
{
    mPos = Vector2::Zero;
    mLookUp = false;
    mLookDown = false;
    mOffset = Vector2::Zero;
    mDistMove = 200;
    mTimerToStartLooking = 0.0f;
    mLookDelay = 1.2f;
}

// void Camera::Update(float deltatime) {
//     Vector2 playerPos = mGame->GetPlayer()->GetPosition();
//     Vector2 basePos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight()/2);
//
//     // Define o alvo do deslocamento (offset)
//     Vector2 targetOffset(0.0f, 0.0f);
//     if (mLookUp) {
//         mTimerToStartLooking += deltatime;
//         if (mTimerToStartLooking >= mLookDelay) {
//             targetOffset.y = -mDistMove;
//         }
//     } else if (mLookDown) {
//         mTimerToStartLooking += deltatime;
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
//         mOffset.x + (targetOffset.x - mOffset.x) * mCameraLerpSpeed * deltatime,
//         mOffset.y + (targetOffset.y - mOffset.y) * mCameraLerpSpeed * deltatime
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
//     if (mPos.y > mGame->GetWindowHeight()) {
//         mPos.y = mGame->GetWindowHeight();
//     }
// }

void Camera::Update(float deltatime) {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    Vector2 targetPos(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight()/2);

    // Aplica deslocamento vertical se estiver olhando para cima ou para baixo
    if (mLookUp) {
        mTimerToStartLooking += deltatime;
        if (mTimerToStartLooking >= mLookDelay) {
            targetPos.y -= mDistMove;
        }
    }
    else if (mLookDown) {
        mTimerToStartLooking += deltatime;
        if (mTimerToStartLooking >= mLookDelay) {
            targetPos.y += mDistMove;
        }
    }

    if (!mLookUp && !mLookDown) {
        mTimerToStartLooking = 0.0f;
    }

    // Sempre interpola a posição atual da câmera para a posição-alvo
    mPos = Vector2(
        mPos.x + (targetPos.x - mPos.x) * mCameraLerpSpeed * deltatime,
        mPos.y + (targetPos.y - mPos.y) * mCameraLerpSpeed * deltatime
    );

    // Reset flags para o próximo frame
    mLookUp = false;
    mLookDown = false;

    // Trava a câmera abaixo de certo valor
    if (mPos.y > mGame->GetWindowHeight()) {
        mPos.y = mGame->GetWindowHeight();
    }
}



