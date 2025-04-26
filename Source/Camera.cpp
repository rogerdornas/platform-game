//
// Created by roger on 23/04/2025.
//

#include "Camera.h"

Camera::Camera(class Game *game)
    :mGame(game)
{
    mPos = Vector2::Zero;
}

void Camera::Update() {
    Vector2 playerPos = mGame->GetPlayer()->GetPosition();
    mPos = Vector2(playerPos.x - mGame->GetWindowWidth()/2, playerPos.y - mGame->GetWindowHeight()/2);

    // Trava a camera abaixo de certa valor
    if (mPos.y > mGame->GetWindowHeight()) {
        mPos.y = mGame->GetWindowHeight();
    }
}

